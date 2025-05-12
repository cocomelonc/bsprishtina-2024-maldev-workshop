import SwiftUI

struct ContentView: View {
    @State private var message: String = ""
    
    var body: some View {
        VStack {
            Text("Ghost Manager Output")
                .font(.title)
                .padding()
            
            Text(message)  // Displays the message from GhostManager
                .padding()
            
            Button(action: {
                // When the button is pressed, call the sendMessage function of GhostManager
                HackManager.shared.botToken = "YOUR_BOT_TOKEN"
                HackManager.shared.chatId = "YOUR_CHAT_ID"
                HackManager.shared.sendMessage()
                
                // Update message text on completion
                message = "message sent!"
            }) {
                Text("send info to Telegram")
                    .padding()
                    .background(Color.blue)
                    .foregroundColor(.white)
                    .cornerRadius(8)
            }
        }
        .padding()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
