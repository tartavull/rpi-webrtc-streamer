/*
Copyright (c) 2018, rpi-webrtc-streamer Lyu,KeunChang

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <map>
#include <string>
#include "rtc_base/logging.h"

#include "utils_pc_config.h"
#include "utils_pc_strings.h"

static const char kConfigDelimiter=',';
static const char kStunPrefix[] = "stun:";
static const char kTurnPrefix[] = "turn:";
static const char kDefaultStunServer[] = "stun:stun.l.google.com:19302";

namespace utils {

TransportsType ConfigToIceTransportsType(const std::string type) {
    std::map<std::string, TransportsType> keyword_map;
    keyword_map["none"] = TransportsType::kNone;
    keyword_map["relay"] = TransportsType::kRelay;
    keyword_map["nohost"] = TransportsType::kNoHost;
    keyword_map["all"] = TransportsType::kAll;
    if(keyword_map.find(type) != keyword_map.end())
        return keyword_map[type];
    else {
        RTC_LOG(LS_ERROR) << __FUNCTION__ << "TransportsType : " << type
            << " not found, using default all";
        return TransportsType::kAll;
    }
}

BundlePolicy ConfigToIceBundlePolicy(const std::string bundle_policy){
    std::map<std::string, BundlePolicy> keyword_map;
    keyword_map["balanced"] = BundlePolicy::kBundlePolicyBalanced;
    keyword_map["max-bundle"] = BundlePolicy::kBundlePolicyMaxBundle;
    keyword_map["max-compat"] = BundlePolicy::kBundlePolicyMaxCompat;
    if(keyword_map.find(bundle_policy) != keyword_map.end())
        return keyword_map[bundle_policy];
    else {
        RTC_LOG(LS_ERROR) << __FUNCTION__ << "BundlePolicy : " << bundle_policy
            << " not found, using default balanced";
        return BundlePolicy::kBundlePolicyBalanced;
    }
}

MuxPolicy ConfigToIceRtcpMuxPolicy(const std::string mux_policy) {
    std::map<std::string, MuxPolicy> keyword_map;
    keyword_map["require"] = MuxPolicy::kRtcpMuxPolicyRequire;
    keyword_map["negotiate"] = MuxPolicy::kRtcpMuxPolicyNegotiate;
    if(keyword_map.find(mux_policy) != keyword_map.end())
        return keyword_map[mux_policy];
    else {
        RTC_LOG(LS_ERROR) << __FUNCTION__ << "MuxPolicy : " << mux_policy
            << " not found, using default require";
        return MuxPolicy::kRtcpMuxPolicyRequire;
    }
}

CertPolicy ConfigToIceTlsCertPolicy(const std::string cert_poicy){
    std::map<std::string, CertPolicy> keyword_map;
    keyword_map["secure"] = CertPolicy::kTlsCertPolicySecure;
    keyword_map["insecure"] = CertPolicy::kTlsCertPolicyInsecureNoCheck;
    if(keyword_map.find(cert_poicy) != keyword_map.end())
        return keyword_map[cert_poicy];
    else {
        RTC_LOG(LS_ERROR) << __FUNCTION__ << "CertPolicy : " << cert_poicy
            << " not found, using default secure";
        return CertPolicy::kTlsCertPolicySecure;
    }
}

std::vector<std::string> ConfigToIceUrls(const std::string urls) {
    std::vector<std::string> url_list;
    std::stringstream ss(urls);
    std::string token;

    while( getline(ss, token, kConfigDelimiter) ) {
        // only compare "stun:" prefix
        if( token.compare(0, 5 /* kStunPrefix len */, kStunPrefix) == 0 )  {
            url_list.push_back(token);
        }
        else if( token.compare(0, 5 /* kTurnPrefix len */, kTurnPrefix) == 0 )  {
            url_list.push_back(token);
        }
    }
    return url_list;
}

std::vector<std::string> ConfigToVector(const std::string configs) {
    std::vector<std::string> config_list;
    std::stringstream ss(configs);
    std::string token;

    while( getline(ss, token, kConfigDelimiter) ) {
        config_list.push_back(token);
    }
    return config_list;
}

void PrintIceServers(const RTCConfiguration &rtc_config){
    RTC_LOG(INFO) << "RTC Configuration for ICE and ICE servers";
    RTC_LOG(INFO) << "- TransportsType : "
        << utils::IceTransportsTypeToString(rtc_config.type);
    RTC_LOG(INFO) << "- BundlePolicy : "
        << utils::BundlePolicyToString(rtc_config.bundle_policy);
    RTC_LOG(INFO) << "- RtcpMuxPolicy : "
        << utils::RtcpMuxPolicyToString(rtc_config.rtcp_mux_policy);
    RTC_LOG(INFO) << "- ICE server(s) : "  << rtc_config.servers.size();

    int server_index = 0;
    for (const webrtc::PeerConnectionInterface::IceServer& server :
            rtc_config.servers) {
        if (!server.urls.empty()) {
            RTC_LOG(INFO) << "- ICE server : #" << server_index;
            for (const std::string& url : server.urls) {
                if (url.empty()) {
                    RTC_LOG(INFO) << "-- url : "
                        << "Error: url is empty";
                }
                else {
                    RTC_LOG(INFO) << "-- url : " << url;
                }
            }

            if( !server.username.empty())
                RTC_LOG(INFO) << "-- username : " << server.username;
            if( !server.password.empty())
                RTC_LOG(INFO) << "-- password : " << server.password;
            if( !server.hostname.empty())
                RTC_LOG(INFO) << "-- hostname : " << server.hostname;
            if( !server.password.empty())
            RTC_LOG(INFO) << "-- Tls Cert Policy : "
                << utils::TlsCertPolicyToString(server.tls_cert_policy);
            if( server.tls_alpn_protocols.size() ) {
                RTC_LOG(INFO) << "-- Tls Alpn Protocols : "
                    << server.tls_alpn_protocols.size();
                for (const std::string& protocols : server.tls_alpn_protocols) {
                    RTC_LOG(INFO) << "--- Alpn Protocols : "  <<  protocols;
                }
            };
            if(server.tls_elliptic_curves.size()) {
                RTC_LOG(INFO) << "-- Tls Elliptic Curves : "
                    << server.tls_elliptic_curves.size();
                for (const std::string& curves : server.tls_elliptic_curves) {
                    RTC_LOG(INFO) << "--- Curves : "  <<  curves;
                }
            }
        }
        else {
            RTC_LOG(INFO) << "- ICE server : #" << server_index
                << "Syntax error, urls is empty";
        }
        server_index++;
    }
}

};  // utils namespace


