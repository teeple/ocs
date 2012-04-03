<?php
/**
 * @file
 *  Link general private message module functionalities to services module.
 */

function privatemsg_service_get_by_phoneno($offset = 0, $limit = 1, $recipient_phoneno, $sender_phoneno) {

    /* paramters */
    $type = 'inbox';
    $load_full = FALSE;
    $offset = 0;
    $limit = 1;

    $recipient_uid = _get_uid_by_phoneno($recipient_phoneno);
    $account_tmp = user_load($recipient_uid);

    $query = _privatemsg_assemble_query('list', $account_tmp, 'inbox');

    $threads = array();
    foreach($query->execute() as $thread) {
        $threads[] = $thread;
    }

    if(sizeof($threads) > 0) {

        $messages = privatemsg_thread_load($threads[0]->thread_id,$account_tmp);

        $message = end($messages['messages']);

        $return = array('mid' => $mesage->mid,
                'sender_phoneno' => $message->author->field_phoneno['und'][0]['value'],
                'recipient_phoneno' => $recipient_phoneno,
                'body' => $message->body);

        return $return;
    }

    $ret = array('success' => false);
    return $ret;
}

function privatemsg_service_get_by_mid($mid, $recipient_phoneno) {

    $recipient_uid = _get_uid_by_phoneno($recipient_phoneno);
    $ret = privatemsg_message_load($mid, user_load($recipient_uid));

    return $ret;
}

function privatemsg_service_send_by_phoneno($sender_phoneno,$recipient_phoneno_list, $body, $subject){

    error_log('global user: '.$user->uid);

    $ret_arr = array();
    $ret;

    $senderUid = _get_uid_by_phoneno($sender_phoneno);

    $account_tmp = user_load($senderUid);
    $recipient_uid = 0;

    $threads = array();
    $query = _privatemsg_assemble_query('list', $account_tmp, 'all');

    foreach($query->execute() as $thread) {
        $threads[] = $thread;
    }

    $recipients = explode(',', $recipient_phoneno_list);

    foreach($recipients as $phoneno) {

        $recipient_uid = _get_uid_by_phoneno($phoneno);
        $found_thread_flag = 0;

        $found_flag = FALSE;

        foreach ($threads as $thread) {

           error_log('thread: '.(string)$thread->participants);

           if( !is_null($thread->participants)
                &&($thread->participants == 'user_'.(string)$recipient_uid))
           {
                $found_flag = TRUE;
               break;
           }
        }

        if ($found_flag) {
            error_log('thread found: thread_id - '.(string)$thread->thread_id);
            $ret = privatemsg_reply($thread->thread_id, $body, array());

        } else {

            error_log('thread not found');
            error_log('recipient_uid: '.(string)$recipient_uid);
            error_log('body: '.$body);
            $subject = 'SMS MESSAGE from PRIVATEMSG_SERVICE';

            $recipient = user_load($recipient_uid);

            $ret =  privatemsg_new_thread(array($recipient), $subject, $body);
       }
       $ret_arr[] = $ret;
    }

    $ret = array('success' => true);
    return $ret;
}

