<?php
$ws = new WebSocketServer();
$ws->bind("0.0.0.0", "8080");
$ws->onopen = function($client) {
  echo "New client connection: " . $client->sockfd . "\n";
};
$ws->onclose = function($client) {
  echo "Client closed connection: " . $client->sockfd . "\n";
};
$ws->onmessage = function($client, $msg) {
  switch($msg->opcode) {
    case WebSocketMessage::OPCODE_TEXT:
      echo "Received text message '" . $msg->payload . "' from client.\n";
      echo "Sending it back...\n";
      $client->sendText($msg->payload);
      break;
    case WebSocketMessage::OPCODE_BINARY:
      echo "Received binary message of length (" . $msg->payload_len . ") from client.\n";
      echo "Sending it back...\n";
      $client->sendBinary($msg->payload, $msg->payload_len);
      break;
  }
};
$ws->run();
?>
