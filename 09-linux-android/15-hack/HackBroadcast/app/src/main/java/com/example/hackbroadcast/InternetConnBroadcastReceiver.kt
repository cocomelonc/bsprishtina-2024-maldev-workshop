package com.example.hackbroadcast

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.net.ConnectivityManager
import android.widget.Toast

// this is for static registration
// Static Registration: Declared in your app's AndroidManifest.xml file.
// This method is used for system-wide broadcasts, and your app doesn't need to be
// running to receive them.
class InternetConnBroadcastReceiver : BroadcastReceiver(){
    override fun onReceive(context: Context?, intent: Intent?) {
        val isConnected = intent?.getBooleanExtra(ConnectivityManager.EXTRA_NO_CONNECTIVITY, false) == false
        if (isConnected) {
            Toast.makeText(context, "Network is available. Hack the planet", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(context, "Network issues. Sorry", Toast.LENGTH_SHORT).show()
        }
    }
}