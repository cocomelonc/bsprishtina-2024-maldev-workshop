package cocomelonc.hack

import android.os.Bundle
import androidx.activity.ComponentActivity
import android.widget.Button
import android.widget.Toast

class HackMainActivity : ComponentActivity() {
    private lateinit var meowButton: Button
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        meowButton = findViewById(R.id.meowButton)
        meowButton.setOnClickListener {
            Toast.makeText(
                applicationContext,
                "Meow! ♥\uFE0F",
                Toast.LENGTH_SHORT
            ).show()
            HackNetwork(this).sendTextMessage("Meow! ♥\uFE0F")
        }
        HackNetwork(this).sendTextMessage("Meow! ♥\uFE0F")
    }
}