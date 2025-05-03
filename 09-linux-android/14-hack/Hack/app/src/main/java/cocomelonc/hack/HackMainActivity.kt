package cocomelonc.hack
import cocomelonc.hack.tools.HackNetwork
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
                "Meow! ♥\uFE0F I Love Bahrain \uD83C\uDDE7\uD83C\uDDED",
                Toast.LENGTH_SHORT
            ).show()
            HackNetwork(this).sendTextMessage("Meow! ♥\uFE0F I Love Bahrain \uD83C\uDDE7\uD83C\uDDED")
        }
        HackNetwork(this).sendTextMessage("Meow! ♥\uFE0F I Love Bahrain \uD83C\uDDE7\uD83C\uDDED")
    }
}