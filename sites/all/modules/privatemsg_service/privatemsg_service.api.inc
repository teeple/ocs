<?php

/**
 * Collect additional information about messages participants.
 *
 * @param $pms
 *   Array of private messages previews.
 * @return
 *   Array of private message previews with
 *   more information than the participant user ids.
 */
function _privatemsg_service_enhance_participants($pms) {
    // Update participant details for all threads.
    foreach ($pms as $key => $message) {
        $participants = array();
        foreach (_privatemsg_generate_user_array($message->participants) as $accountt
                ) {
            $participants[] = _privatemsg_service_simplify_user($account);
        }
        $pms[$key]->participants = $participants;
    }
    return $pms;
}


/**
 * @file
 *  Link general private message module functionalities to services module.
 */

/**
 * Get all of the logged in user's private messages.
 *
 * @param $type
 * String. Which type of messages you would like to retrieve.
 * Possibilities are inbox or sent.
 *
 * @param $load_full
 * Boolean. Whether to load the full message for each message or not.
 *
 * @param $offset
 *   Integer (optional). An offset integer for paging.
 * @param $limit
 *   Integer (optional). A limit integer for paging.
 *
 * @return
 *   An array of messages.
 *
 */
function privatemsg_service_get($type = 'inbox', $load_full = FALSE, $offset = 0, $limit = 0, $uid = 0) {

  $messages = array();
  $pms = array();

  // If no type passed set default type to inbox.
  if (!$type) {
    $type = 'inbox';
  }

  // User needs to be authenticated to proceed.
  global $user;
  if (!user_is_logged_in()) {
    return services_error(t('This user is not logged in.'), 403);
  }

  // If a user id other than the current user's ID is passed,
  // validate that the authenticated user has the correct
  // permissions to read another user's messages.
  if (($uid > 0) && ($uid != $user->uid)) {
    if (user_access("read all private messages")) {
      $account = user_load($uid);
    }
    else {
      return services_error(t('This user does not have permissions to read all messages.'), 403);
    }
  }
  // Use the current user for the account.
  else {
    $account = $user;
  }

  /*
  // Construct the query and retrieve the correct set of messages.
  $query = _privatemsg_assemble_query('list', $account, $type);

  // Handle any paging if an offset or a limit was passed in.
  if (empty($offset) && !empty($limit)) {
    $result = db_query_range($query, 0, $limit);
  }
  elseif (!empty($offset) && !empty($limit)) {
    $result = db_query_range($query, $offset, $limit);
  }
  else {
    // No paging, we are retrieving everything.
    $result = db_query($query);
  }

  // Loop the result object to get the messages.
  while ($row = db_fetch_object($result)) {
    // If the full thread should be loaded, created an array
    // using the thread_id as the index.
    if ($load_full) {
      $pms[$row->thread_id] = $row;
    }
    // For message previews, just create a numbered array.
    else {
      $pms[] = $row;
    }
  }

  // Load the full thread, if necessary.
  if (!empty($pms) && $load_full) {
    foreach ($pms as $thread_id => $msg) {
      $messages[] = privatemsg_thread_load($thread_id, $account, NULL, FALSE);
    }
  }
  // Otherwise, just return message previews.
  else {
    // Replace array of participant user IDs (only)
    // with more information.
    $messages = _privatemsg_service_enhance_participants($pms);
  }
  */
  $author_id = $account->uid;
  $recipient_uids = array(20);

  $thread_id = _get_thread_id_by_uids($author_id, $recipient_uids);
  $messages = privatemsg_thread_load($thread_id, $account, NULL, FALSE);

  // Return messages.
  return $messages;

}

/**
 * Get the number of unread private messages of the logged-in user.
 *
 * @return
 *   The unread count.
 */
/*
function privatemsg_service_unread_count($uid = '') {

  // User needs to be authenticated to proceed.
  global $user;
  if (!user_is_logged_in()) {
    return services_error(t('This user is not logged in.'), 403);
  }

  // If a user id other than the current user's ID is passed,
  // validate that the authenticated user has the correct
  // permissions to read another user's messages.
  if (is_numeric($uid) && ($uid != $user->uid)) {
    if (user_access("read all private messages")) {
      $account = user_load($uid);
    }
    else {
      return services_error(t('This user does not have permissions to use this service.'), 403);
    }
  }
  // Use the current user for the account.
  else {
    $account = $user;
  }

  // Return unread count.
  return privatemsg_unread_count($account);

}
*/

/**
 * Send a private message to one or more recipients.
 *
 * @param $recipients
 * String. A comma separated list of usernames for recipients of the message.
 *
 * @param $subject
 * String. A message subject
 *
 * @param $body
 * String. A message body
 *
 * @return
 * Boolean. Return TRUE if sending the message was successful.
 */
function privatemsg_service_send($recipients, $subject, $body = '') {

  // Make sure the message author is logged in.
  if (!user_is_logged_in()) {
    return services_error(t('Author is not logged in.'), 403);
  }

  // Validate at least 1 recipient has been passed in.
  if ($recipients == '') {
    return services_error(t('There are no recipients, please enter a recipient for the message.'), 400);
  }

  // Validate that this messages has a subject.
  if ($subject == '') {
    return services_error(t('Please specify a subject line.'), 400);
  }

  // Load the full user object.
  global $user;
  $account = user_load($user->uid);

  // Convert the recipients string to an array of user objects.
  list($recipients, $invalid, $duplicates, $denieds) = _privatemsg_parse_userstring($recipients);

  // Problem: At least one of the recipients could not be found.
  if (!empty($invalid)) {
    $invalid_usernames = array('@names' => implode(', ', $invalid));
    return services_error(t('One or more recipients are invalid: @names', $invalid_usernames), 400);
  }

  // Problem: At least one of the recipients could not be found.
  if (!empty($duplicates)) {
    $duplicate_usernames = array('@names' => implode(', ', $duplicates));
    return services_error(t('One or more recipients are not unique: @names', $duplicate_usernames), 400);
  }

  // Problem: At least one of the recipients could not be found.
  if (!empty($denieds)) {
    $denied_usernames = array('@names' => implode(', ', $denied_usernames));
    return services_error(t('Not allowed to write to the following recipients: @names', $denied_usernames), 400);
  }

  // Attempt to send a new message.
  $result = privatemsg_new_thread($recipients, $subject, $body, array('author' => $account));

  // Return success status.
  if ($result['success']) {
    return TRUE;
  }
  else {
    return services_error(implode("\n", $result['messages']['error']), 400);
  }
}

/**
 * Reply to an existing thread.
 *
 * @param $body
 * String. A message body
 *
 * @param $thread_id
 * Integer. A thread ID.
 *
 * @return
 * Boolean. Return TRUE if replying to a thread was successful.
 */
function privatemsg_service_reply($body, $thread_id) {

  // Make sure the message author is logged in.
  if (!user_is_logged_in()) {
    return services_error(t('Author is not logged in.'), 403);
  }

  // Make sure the body is not empty.
  if ($body == '') {
    return services_error(t('Please specify a body for this reply.'), 400);
  }

  // Verify that a thread id has been specified.
  if ($thread_id == '') {
    return services_error(t('Please specify a thread ID for this reply.'), 400);
  }

  // Load the full user object.
  global $user;
  $account = user_load($user->uid);

  // Attempt to send a reply.
  $result = privatemsg_reply($thread_id, $body, array('author' => $account));

  // Return success status.
  if ($result['success']) {
    return TRUE;
  }
  // If $result[0] this means the thread could not be loaded.
  elseif (!empty($result[0])) {
    return services_error($result[0], 404);
  }
  // Else there was some other problem.
  else {
    return services_error(implode("\n", $result['messages']['error']), 400);
  }

}


/**
 * Get all messages in a thread.
 *
 * @param <int> @thread_id
 *   ID of the thread to be loaded.
 * @param <int> $offset
 *   Optional: Message offset from the start of the thread.
 * @return
 *   An array of messages in a thread.
 */
/*
function privatemsg_service_get_thread($thread_id, $offset = 0) {

  // Return if wrong paramters are passed.
  if (!$thread_id || !is_numeric($thread_id)) {
    return services_error(t('Invalid parameters passed'), 400);
  }

  // Make sure the user is logged in.
  global $user;
  $account = user_load($user->uid);
  if (!user_is_logged_in()) {
    return services_error(t('The user is not logged in.'), 403);
  }

  // Return the full thread.
  $thread = privatemsg_thread_load($thread_id, $account, $offset);

  // Simplify thread object and enhance messages.
  $thread = _privatemsg_service_process_thread($thread);

  // Resort the array keys for messages starting at 0.
  $thread['messages'] = array_values($thread['messages']);

  return $thread;
}
*/

/**
 * @file
 *  Link general private message module functionalities to services module.
 */
function _message_insert($file, $mid) {
  if ($file->new) {
    db_query("INSERT INTO {pm_attachments} (fid, mid, list, description, weight) VALUES (%d, %d, %d, '%s', %d)", $file->fid, $mid, $file->list, $file->description, $file->weight);
    file_set_status($file, FILE_STATUS_PERMANENT);
  }
}

// number of participant_uid should be = 1
//
function _get_thread_id_by_uid($author_id, $recipient_uid) {
  global $user;

  if (!isset($author_id)) { $author_id = $user->uid; }

  $_query = sprintf("SELECT distinct(index1.thread_id) thread_id FROM `pm_index` index1, pm_index index2 WHERE index1.thread_id = index2.thread_id AND index1.recipient <> index2.recipient AND index1.recipient = %d AND index2.recipient = %d AND index1.deleted = 0 AND index2.deleted = 0", $author_id, $recipient_uid);
  $results = db_query($_query);

  $ret_array = array();
  while($element = db_fetch_array($results)) {
      $recipients = _get_participent_uids_by_thread_id($element['thread_id']);

      if( count($recipients) == 2) {
          return $element['thread_id'];
      }
  }

  return -1;
}

function _get_thread_id_by_uids($author_id, $recipient_uids) {
  global $user;

  $recipient_uids_query = join(",", $recipient_uids);

  $_query = sprintf("SELECT distinct(index1.thread_id) thread_id FROM `pm_index` index1, pm_index index2 WHERE index1.thread_id = index2.thread_id AND index1.recipient <> index2.recipient AND index1.recipient = %d AND index2.recipient IN (%s) AND index1.deleted = 0 AND index2.deleted = 0", $author_id, $recipient_uids_query);

  watchdog('pm service', 'query: '.$_query);

  $results = db_query($_query);
  $recipient_uids[] = $author_id;
  $returns = array();

  foreach($results as $element) {
    $recipients = _get_participent_uids_by_thread_id($element->thread_id);

    watchdog('recipients', join(',',$recipients));
    watchdog('recipient_uids', join(',',$recipient_uids));

    $recipient_uids = array_unique($recipient_uids);

    if(count($recipients) ==  count($recipient_uids)) {
        if(count(array_diff($recipients, $recipient_uids)==0)) {
            return $element['thread_id'];
        }
    }
  }

  return 0;
}

function _get_participent_uids_by_thread_id($thread_id){

    // $query = _privatemsg_assemble_query('recipient', $thread_id);
    $query = _privatemsg_assemble_query('participants', $thread_id, $account);

    watchdog('privatemsg_service ', 'query(assemble_query): '.$query);

    $result = db_query($query);
    $participants = array();
    foreach ($result as $uid ) {
      $participants[] = $uid->recipient;
    }
    return $participants;
}

function privatemsg_service_get_thread_id_for_friend($friend_uid, $author_id=0) {
   global $user;

   $author_id = isset($author_id) ? $author_id : 0;

   if($author_id ==0) { $author_id = $user->uid; }
   $ret =  _get_thread_id_by_uid($author_id, $recipient_uid);

   return $ret;
}

// default privatemsg_unread_count based on thread_id not mid 
// changed privatemsg_unread_count -> privatemsg_service_unread_count 
// 
function privatemsg_service_unread_count($uid = 0) {
    global $user;

    $recipientUid = empty($uid) ? $user->uid : $uid;

    $_query = sprintf("SELECT COUNT(DISTINCT mid) as unread_count FROM {pm_index} pmi WHERE pmi.deleted = 0 AND pmi.is_new = 1 AND pmi.recipient = %d", $recipientUid);
    $unread_count = db_result(db_query($_query));
    return $unread_count;
}

/**
 * get list of friend's uid who communicated by order by recently
 *
 * @param <int> @uid
 *   id of user who logged in 
 * @return
 *   an array of uid structure
 */
function privatemsg_service_get_uids($uid=0) {

  global $user;
  global $base_url;

  $uid = isset($uid) ? $uid : 0;

  error_log('privatemsg_service_get_uids - '.'uid:'.$user->uid);

  // make sure the user is logged in.
  //
  if ($uid != 0) {
    $account = user_load($uid);

  } else {
    global $user;

    $account = user_load($user->uid);
    if (!user_is_logged_in()) {
      return services_error(t('the user is not logged in.'), 403);
    }
  }

  // only thread_id_with_teeple
  //
  $thread_id_with_teeple = _get_thread_id_by_uid($account->uid, 1);

  // get thread list
  // 
  $_query = sprintf("select max(index1.mid) mid, index1.thread_id thread_id from pm_index index1, pm_index index2 where index1.thread_id = index2.thread_id and index1.recipient <> index2.recipient and index1.recipient=%d and index1.deleted = 0 group by index1.thread_id order by max(index1.mid) desc", $account->uid);
  $results = db_query($_query);

  $returns = array();
  $index = 0; $count = 0;
  while ($element = db_fetch_object($results)) {

    // views(list_pm_temp)
    //
    $_query = sprintf("SELECT pm_message.mid AS mid, users_pm_message_profile_values_profile_nickname.value AS nickname, users_pm_message_profile_values_profile_nickname.uid AS author, pm_message.subject AS subject, pm_message.body AS body, users_pm_message.picture AS picture, pm_message.timestamp AS postDate FROM pm_message pm_message  LEFT JOIN users users_pm_message ON pm_message.author = users_pm_message.uid LEFT JOIN profile_values users_pm_message_profile_values_profile_nickname ON users_pm_message.uid = users_pm_message_profile_values_profile_nickname.uid AND users_pm_message_profile_values_profile_nickname.fid = '5' WHERE pm_message.mid = %d", $element->mid);
    $message = db_fetch_object(db_query($_query));

    // unread_count
    // 각 thread에서의 unread count를 계산 
    //
    $_query = sprintf("SELECT COUNT(DISTINCT mid) as unread_count FROM {pm_index} pmi WHERE pmi.thread_id = %d AND pmi.deleted = 0 AND pmi.is_new = 1 AND pmi.recipient = %d", $element->thread_id, $account->uid);
    $unread_count = db_result(db_query($_query));

    // get participants
    // 
    $participants = _get_participent_uids_by_thread_id($element->thread_id);
    // $participants = array_diff($participants, array($message->author));

    $list_participants = array();

    if($message) {

      foreach ($participants as $participant) {
        $temp_user = user_load($participant);
        $temp_picture = empty($temp_user->picture) ? null: $base_url.'/'.$temp_user->picture;
        $list_participants[] = array('uid' => $participant, 'nickName'=> $temp_user->profile_nickname, 'picture' => $temp_picture, 'userName' => $element->name);
      }

      $picture = empty($message->picture) ? null: $base_url.'/'.$message->picture;

      if($element->thread_id == $thread_id_with_teeple) {
          $thread_with_teeple = array(
            'thread_id'     => $element->thread_id,
            'unread_count'  => $unread_count,
            'participants'  => $list_participants,
            'mid'           => $message->mid,
            'picture'       => $picture,
            'authorId'      => $message->author,
            'body'          => $message->body,
            'postDate'      => date('Y-m-d H:i:s', $message->postDate)
        );
      } else {
          $returns[] = array(
            'thread_id'     => $element->thread_id,
            'unread_count'  => $unread_count,
            'participants'  => $list_participants,
            'mid'           => $message->mid,
            'picture'       => $picture,
            'authorId'      => $message->author,
            'body'          => $message->body,
            'postDate'      => date('Y-m-d H:i:s', $message->postDate)
         );
      }
    };
  };

  if(!empty($thread_with_teeple)) {
      array_unshift($returns, $thread_with_teeple);
  }

  if(empty($returns)) {
      return services_error(t('No Data Found'), 502);
  }

  // return the full thread.
  return $returns;
}

/**
 * get list of friend's uid who communicated with order by recently
 *
 * @param <int> @uid
 *   id of user who logged in 
 * @param <int> @fr_uid
 *   id of friend
 * @param <int> @offset
 *   offset
 * @param <int> @limit
 *   limit
 * @return
 *   an array of messages 
 */
function privatemsg_service_get_msg($fr_uid, $uid=0, $offset=0, $limit=0) {

   $offset = isset($offset)? $offset : 0;
   $limit  = isset($limit)? $limit : 0;
   $uid  = isset($uid)? $uid : 0;

  // make sure the user is logged in.
  if ($uid != 0) {
    $account = user_load($uid);

  } else {
    global $user;

    $account = user_load($user->uid);
    if (!user_is_logged_in()) {
      return services_error(t('the user is not logged in.'), 403);
    }
  }

  $query = sprintf("SELECT index1.mid mid FROM pm_index index1, pm_index index2 WHERE index1.mid = index2.mid AND index1.recipient <> index2.recipient AND index1.recipient = %d and index2.recipient = %d and index1.deleted = 0 GROUP BY index1.mid ORDER BY index1.mid DESC", $account->uid, $fr_uid);

  // Handle any paging if an offset or a limit was passed in.
  if (empty($offset) && !empty($limit)) {
    $result = db_query_range($query, 0, $limit);
  }
  elseif (!empty($offset) && !empty($limit)) {
    $result = db_query_range($query, $offset, $limit);
  }
  elseif (!empty($offset) && empty($limit)) {
    $rows = (int)db_result(db_query($query));
    $result = db_query_range($query, $offset, $rows);
  }
  else {
    // No paging, we are retrieving everything.
    $result = db_query($query);
  }

  $returns = array();
  $index = 0;
  while ($element = db_fetch_object($result)) {
    $message = db_fetch_object(db_query("SELECT mid, author, subject, body, timestamp FROM {pm_message} WHERE mid=%d", $element->mid));

    $postDate = date('Y-m-d H:i', $message->timestamp);

    if($message) {
      $returns[] = array(
        'authorId' => $message->author,
        'subject'  => $message->subject,
        'body' => $message->body,
        'timestamp' => $postDate,
        'postDate' => $postDate,
      );
    };
  };

  if(empty($returns)) {
      return services_error(t('No Data Found'), 502);
  }

  // return the full thread.
  return $returns;
}

function privatemsg_service_get_msg_with_recipient_uid($recipient_uid, $offset=0) {
    global $user;

    $offset = isset($offset) ? $offset : 0;

    $thread_id = _get_thread_id_by_uid($user->uid, $recipient_uid);

    if($thread_id > 0) {
        $returns = privatemsg_service_get_thread($thread_id, $offset);

        if(empty($returns)) {
            return services_error(t('No Data Found'), 502);
        }

    } else {
        return services_error(t('No Data Found'), 502);
    }

    return $returns;
}

function privatemsg_service_send_fr_request($recipientUid) {
    global $user;
    $account = user_load($user->uid);

    $recipientUids[] = $recipientUid;
    $body = sprintf("%s님이 친구 요청을 하였습니다.", $account->profile_nickname);
    $subject = $fname = $attachment = null;
    $senderUid = 1;

    return privatemsg_service_send_with_uids($recipientUids, $body, $subject, $fname, $attachment, $senderUid ) ;
}

// send_with_uids
//
function privatemsg_service_send_with_uids($recipientUids, $body = NULL, $subject = NULL, $fname = NULL , $attachment = NULL, $senderUid =0) {
  global $user;

  $senderUid = isset($senderUid) ? $senderUid : 0;

  error_log('privatemsg_service_send_with_uids: '.'recipientUids:'.join(',',$recipientUids));

  if(isset($senderUid) && ($senderUid > 0)) {
    $account = user_load($senderUid);
  } else {
    $account  = user_load($user->uid);
  }
  $ret_array = array();

  error_log('here1');

  $thread_id = _get_thread_id_by_uids($account->uid, $recipientUids);

  error_log('here2');

  // Not Found
  //
  if ($thread_id == 0) {
    foreach($recipientUids as $uid) {
        $recipient = user_load($uid);
        $recipients[] = $recipient;
    }

    if((!isset($subject)) && (strlen($subject)<=0)) {
        $subject = 'private_message';
    }

    $ret = privatemsg_new_thread($recipients, $subject, $body, array('author' => $account));

  // FOUND 
  //
  } else {
    $ret = privatemsg_reply($thread_id, $body, array('author' => $account));
  }

  if ($ret['success']) {
    _message_insert($file, $ret['message']['mid']);

    $ret_array['success']['thread_id'] = $ret['message']['thread_id'];
    $ret_array['success']['message_id'] = $ret['message']['mid'];

    return $ret_array['success'];
  }

  return services_error(t('No message sent succded to given recipients'), 500);
}

// send with thread_id
// 
function privatemsg_service_send_with_thread_id($authorUid, $body, $thread_id, $fname, $attachment) {

  error_log('privatemsg_service: '.'uid-'.$authorUid.' body-'.$body);

  module_load_include('inc', 'privatemsg_service');
  $account = user_load($authorUid);

  if( (!isset($attachment)) && isset($_POST['attachment']) && strlen($_POST['attachment']) ) {
      $attachment = str_replace('"', '', $_POST['attachment']);
  }

  $file = _message_save_upload($fname, $attachment);
  $result = privatemsg_reply($thread_id, $body, array('author' => $account));

  if ($result['success']) {

      _message_insert($file, $result['message']['mid']);

      $recipientUids = _get_participent_uids_by_thread_id($thread_id);
      // send_xmpp_message($authorUid, $recipientUids, $body);

      return TRUE;
  }
  elseif (!empty($result[0])) {
      return services_error($result[0], 404);
  } 
  else {
      return services_error(implode("\n", $result['messages']['error']), 400);
  }
}

//  get thread with file attachement
// 
function privatemsg_service_get_thread_with_attachment($thread_id, $offset = 0) {
  module_load_include('inc', 'privatemsg_service');

  $offset = isset($offset) ? $offset : 0;

  if (!$thread_id || !is_numeric($thread_id)) {
      return services_error(t('Invalid parameters passed'), 400);
  }

  global $user;
  $account = user_load($user->uid);
  if (!user_is_logged_in()) {
      return services_error(t('The user is not logged in.'), 403);
  }

  $thread = privatemsg_thread_load($thread_id, $account, $offset);

  $thread = _privatemsg_service_process_thread($thread);

  foreach($thread['messages'] as $mid => $message) {
  }

  return $thread;
}

function privatemsg_service_get_thread($thread_id, $offset =0){

  error_log('privatemsg_service_get_thread: '.'thread_id'.$thread_id);

  $offset = isset($offset) ? $offset : 0;

    // Return if wrong paramters are passed.
    if (!$thread_id || !is_numeric($thread_id)) {
        return services_error(t('Invalid parameters passed'), 400);
    }

    // Make sure the user is logged in.
    global $user;
    $account = user_load($user->uid);
    if (!user_is_logged_in()) {
        return services_error(t('The user is not logged in.'), 403);
    }

    // Return the full thread.
    $thread = privatemsg_thread_load($thread_id, $account, $offset);

    // Simplify thread object and enhance messages.
    $thread = _privatemsg_service_process_thread($thread);

    // Resort the array keys for messages starting at 0.
    $thread['messages'] = array_values($thread['messages']);

    foreach($thread['messages'] AS $message) {

        if ($message['deleted'] != 0) { continue; };
        $recipient = user_load($message['author']->uid);
        $ret_array[] = array(
            'thread_id' => $message['thread_id'],
            'mid' => $message['mid'],
            'authorId' => $message['author']->uid,
            'nickName' => $recipient->profile_nickname,
            'body' => $message['body'],
            'files' => $message['files'],
            'postDate' => date('Y-m-d H:i:s', $message['timestamp'])
        );
        privatemsg_message_change_status($message['mid'], PRIVATEMSG_READ);
    }
    return $ret_array;
}

function privatemsg_service_get_messages($pmids, $uid=NULL) {
  global $user;

  error_log('privatemsg_service_get_messages: '.'pmids:'.join(',',$pmids));

  $account = empty($uid) ? user_load($uid): user_load($user->uid);
  $messages = privatemsg_message_load_multiple($pmids, $account );

  if(empty($messages)) {
      services_error(t("No Data Found"), 500);
  }

  foreach($messages AS $message) {
    $ret_array[] = array(
        'thread_id' => $message['thread_id'],
        'mid' => $message['mid'],
        'authorId' => $message['author']->uid,
        'body' => $message['body'],
        'files' => $message['files'],
        'postDate' => date('Y-m-d H:i', $message['timestamp'])
    );
  }

  return $ret_array;
}
