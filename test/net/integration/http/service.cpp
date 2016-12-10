// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <service>
#include <net/http/client.hpp>
#include <net/inet4>
#include <info>

std::unique_ptr<http::Client> client_;

void Service::start(const std::string&)
{

}

void Service::ready()
{
  auto& inet = net::Inet4::stack<0>(); // Inet4<VirtioNet>::stack<0>();
  inet.network_config(
    {  10,  0,  0, 44 },  // IP
    {  255,255,255, 0 },  // Netmask
    {  10,  0,  0,  1 },  // Gateway
    {   8,  8,  8,  8 }   // DNS
  );

  client_ = std::make_unique<http::Client>(inet.tcp());

  INFO("Client", "Testing HTTP Client");
  auto req = client_->create_request();

  req->set_uri(uri::URI{"/testing"});

  client_->send(std::move(req), {inet.gateway(), 9000},
  [] (auto err, auto res)
  {
    CHECKSERT(!err, "No error");
    CHECKSERT(res->body() == "/testing", "Received body: \"/testing\"");
    printf("Response:\n%s\n", res->to_string().c_str());
    printf("SUCCESS\n");
  });
}
