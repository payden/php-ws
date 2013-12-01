php-ws
======

PHP extension for writing WebSocket servers in PHP.

[![Build Status](https://travis-ci.org/payden/php-ws.png?branch=master)](https://travis-ci.org/payden/php-ws)

Feel like writing your WebSocket servers in PHP?  Why not.

```php
<?php
$ws = new WebSocketServer();
$ws->bind("0.0.0.0", "8080");
$ws->onopen = function($client) {
  echo "New connection: " . $client->sockfd . "\n";
};
$ws->onclose = function($client) {
  echo "Closing connection: " . $client->sockfd . "\n";
};
$ws->onmessage = function($client, $msg) {
  echo "Received message from client (" . $client->sockfd . "): " . $msg->payload . "\n";
  echo "Sending it back...\n";
  $client->sendText($msg->payload);
};
$ws->run();
?>
```
