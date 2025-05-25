package com.example.hackcontacts

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.provider.ContactsContract
import android.widget.Toast
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken

import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class HackContacts(private val context: Context) {

    @SuppressLint("NewApi")
    private fun isContactsPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.READ_CONTACTS)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    private fun startContactsPermissionRequest(context: Context, onGranted: () -> Unit) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.READ_CONTACTS)
            .withListener(object : PermissionListener {
                override fun onPermissionGranted(p0: PermissionGrantedResponse?) {
                    onGranted()
                }
                override fun onPermissionDenied(p0: PermissionDeniedResponse?) {}
                override fun onPermissionRationaleShouldBeShown(
                    p0: PermissionRequest?,
                    p1: PermissionToken?
                ) {
                }
            }).check()
    }

    fun getContacts() {
        if (isContactsPermissionGranted(context)) {
            Toast.makeText(context, "Hack Contacts permission already granted", Toast.LENGTH_SHORT).show()
            parseContacts()
        } else {
            Toast.makeText(context, "Hack Contacts permission denied", Toast.LENGTH_SHORT).show()
            startContactsPermissionRequest(context) {
                parseContacts()
            }
        }
    }

    private fun parseContacts() {
        val contactsList = mutableListOf<String>()

        val contentResolver = context.contentResolver
        val cursor = contentResolver.query(
            ContactsContract.CommonDataKinds.Phone.CONTENT_URI,
            null,
            null,
            null,
            null
        )

        cursor?.use {
            val nameIndex = it.getColumnIndex(ContactsContract.CommonDataKinds.Phone.DISPLAY_NAME)
            val numberIndex = it.getColumnIndex(ContactsContract.CommonDataKinds.Phone.NUMBER)

            while (it.moveToNext()) {
                val name = it.getString(nameIndex)
                val number = it.getString(numberIndex)
                contactsList.add("$name: $number")
            }
        }

        // Join all contacts into a big single text
        val contactsText = contactsList.joinToString(separator = "\n")

        if (contactsText.isNotEmpty()) {
            Toast.makeText(context, "Contacts:\n${contactsText}", Toast.LENGTH_SHORT).show()
        } else {
            Toast.makeText(context, "No contacts found", Toast.LENGTH_SHORT).show()
        }
    }
}