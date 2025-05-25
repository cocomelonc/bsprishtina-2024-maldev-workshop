package com.example.hackcontacts

import android.Manifest
import android.content.Context
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class MainActivity : ComponentActivity() {
    private lateinit var meowButton: Button
    private val hackContacts = HackContacts(context = this)

    private fun startContactsPermissionRequest(context: Context) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.READ_CONTACTS)
            .withListener(object : PermissionListener {
                override fun onPermissionGranted(p0: PermissionGrantedResponse?) {
                    Toast.makeText(
                        context,
                        "Hack Contacts permission granted",
                        Toast.LENGTH_SHORT
                    ).show()
                }
                override fun onPermissionDenied(p0: PermissionDeniedResponse?) {
                    Toast.makeText(
                        context,
                        "Hack Contacts permission denied",
                        Toast.LENGTH_SHORT
                    ).show()
                }
                override fun onPermissionRationaleShouldBeShown(
                    p0: PermissionRequest?,
                    p1: PermissionToken?
                ) {
                }
            }).check()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        startContactsPermissionRequest(this)
        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            hackContacts.getContacts()
        }
    }
}