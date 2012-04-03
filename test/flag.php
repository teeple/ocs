<?php

/*
 * Server REST - user.login
 */

// REST Server URL
$request_url = 'http://192.168.1.156/note/user/login';

// User data
$user_data = array(
    'username' => 'skytel',
    'password' => 'sky.123',
);
$user_data = http_build_query($user_data);

// cURL
$curl = curl_init($request_url);
curl_setopt($curl, CURLOPT_HTTPHEADER, array('Accept: application/json')); // Accept JSON response
curl_setopt($curl, CURLOPT_POST, 1); // Do a regular HTTP POST
curl_setopt($curl, CURLOPT_POSTFIELDS, $user_data); // Set POST data
curl_setopt($curl, CURLOPT_HEADER, FALSE);  // Ask to not return Header
curl_setopt($curl, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($curl, CURLOPT_FAILONERROR, TRUE);

$response = curl_exec($curl);
$http_code = curl_getinfo($curl, CURLINFO_HTTP_CODE);

// Check if login was successful
if ($http_code == 200) {
      // Convert json response as array
      $logged_user = json_decode($response);
}
else {
      // Get error msg
      $http_message = curl_error($curl);
      die($http_message);
}

/*
 * Server REST - flag.is_flagged
*/

// REST Server URL
$request_url = 'http://192.168.1.156/note/flag/flag';

// Node data
$node_data = array(
    'flag_name' => 'bookmarks',
    'content_id' => 1,
    'action' => 'flag',
    'uid' => 1,
);
$node_data = http_build_query($node_data);

// Define cookie session
$cookie_session = $logged_user->session_name . '=' . $logged_user->sessid;

// cURL
$curl = curl_init($request_url);
curl_setopt($curl, CURLOPT_HTTPHEADER, array('Accept: application/json')); // Accept JSON response
curl_setopt($curl, CURLOPT_POST, 1); // Do a regular HTTP POST
curl_setopt($curl, CURLOPT_POSTFIELDS, $node_data); // Set POST data
curl_setopt($curl, CURLOPT_HEADER, FALSE);  // Ask to not return Header
curl_setopt($curl, CURLOPT_COOKIE, "$cookie_session"); // use the previously saved session
curl_setopt($curl, CURLOPT_RETURNTRANSFER, TRUE);
curl_setopt($curl, CURLOPT_FAILONERROR, TRUE);

$response = curl_exec($curl);
$http_code = curl_getinfo($curl, CURLINFO_HTTP_CODE);

// Check if login was successful
if ($http_code == 200) {
      // Convert json response as array
      $node = json_decode($response);
}
else {
   // Get error msg
    $http_message = curl_error($curl);
    die($http_message);
}
print_r($node);

?>

