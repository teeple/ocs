<?php
/**
 * @file
 * Privatemsg Views module Views hooks.
 */

/**
 * Implementation of hook_views_data_alter().
 */
function privatemsg_views_views_data_alter(&$data) {  
  // ----------------------------------------------------------------
  // pm_message table
  
  $data['pm_message']['table']['group']  = t('Privatemsg');
  $data['pm_message']['table']['base'] = array(
    'field' => 'mid',
    'title' => t('Private message'),
    'help' => t("Private messages sent between users."),
    'weight' => 10,
  );
  $data['pm_message']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'author',
    ),
  );
  
  // mid
  $data['pm_message']['mid'] = array(
    'title' => t('Mesage Id'),
    'help' => t('The ID of the private message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject',
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );
  $data['pm_message']['message_actions'] = array(
    'title' => t('Message actions'),
    'help' => t('Actions related to private messages e.g. "delete".'),
    'real field' => 'mid',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_message_actions',
    ),
  );
  
  // subject
  $data['pm_message']['subject'] = array(
    'title' => t('Subject'),
    'help' => t('The subject of the message.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // body
  $data['pm_message']['body'] = array(
    'title' => t('Body'),
    'help' => t('The body of the message.'),
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'format' => 'format',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // author
  $data['pm_message']['author'] = array(
    'title' => t('Author'),
    'help' => t('Author of the message.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_user_name',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'label' => t('author'),
      'help' => t('Private message authors.'),
      'base' => 'users',
      'base field' => 'uid',
    ),
  );

  // sent
  $data['pm_message']['timestamp'] = array(
    'title' => t('Sent date'),
    'help' => t('The date and time when the message was sent.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  
  // ----------------------------------------------------------------------
  // pm_index table
  
  $data['pm_index']['table']['group']  = t('Privatemsg');
  $data['pm_index']['table']['join'] = array(
    'pm_message' => array(
      'left_field' => 'mid',
      'field' => 'mid',
      'type' => 'INNER',
    ),
  );

  // thread_id
  $data['pm_index']['thread_id'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Message thread'),
    'help' => t('Messages thread ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'subject',
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
  );
  // Thread to message aggregation field.
  $data['pm_index']['thread_msg'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Thread to message id'),
    'help' => t('Field defined by formula to turn message list into thread list.'),
    'real field' => 'thread_id',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_thread_msg',
    ),
  );
  // "Updated" timestamp for the thread.
  $data['pm_index']['updated'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Updated'),
    'help' => t('Last thread update timestamp'),
    'real field' => 'thread_id',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_updated',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    // filtering would require HAVING most likely..
    //'filter' => array(
    //  'handler' => 'views_handler_filter_date',
    //),
  );
  // Pseudo-field to show participants.
  $data['pm_index']['participants'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Participants'),
    'help' => t('Participants.'),
    'real field' => 'thread_id',
    'field' => array(
      'title' => t('Participants'),
      'help' => t('Display user participants.'),
      'handler' => 'privatemsg_views_handler_field_participants',
      'skip base' => 'pm_index',
    ),
  );
  // "number of messages in a thread" pseudo-field.
  $data['pm_index']['count_messages'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Message count'),
    'help' => t('Number of messages in the thread.'),
    'real field' => 'thread_id',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_msg_count',
    ),
  );
  
  
  //------------------------------------------------------------------------
  // These fields require "Thread to message" field to present in the view 
  // Thread message body .
  $data['pm_index']['thread_msg_body'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('Body'),
    'help' => t('The body of the message.'),
    'real field' => 'mid',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_thread_msg_body',
    ),
  );
  // Unread page number.
  $data['pm_index']['unread_pagenum'] = array(
    'group' => t('Privatemsg Thread'),
    'title' => t('First unread page number'),
    'help' => t('Page number of the first unread message in the message thread. If the whole thread is read or returns number of the page containing the message.'),
    'real field' => 'mid',
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_unread_pagenum',
    ),
  );
  

  // uid
  $data['pm_index']['uid'] = array(
    'title' => t('Participant'),
    'help' => t('Show messages where a user is either an author or a recipient.'),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'participant',
      'numeric' => TRUE,
      'validate type' => 'numeric',
    ),
    'relationship' => array(
      'label' => t('participant'),
      'help' => t('Users that are either authors or recipients.'),
      'base' => 'users',
      'base field' => 'uid',
    ),
    'filter' => array(
      'handler' => 'privatemsg_views_handler_filter_participant',
    ),
  );
  
  // is_new
  $data['pm_index']['is_new'] = array(
    'title' => t('New ?'),
    'help' => t('Whether the user has read this message.'),
    'field' => array(
      'handler' => 'privatemsg_views_handler_field_is_new',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
      'click sortable' => TRUE,
    ),
  );

  // deleted
  $data['pm_index']['deleted'] = array(
    'title' => t('Deleted'),
    'help' => t('When the message was deleted by the participant.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'type' => 'yes-no',
      'label' => t('Deleted'),
      'help' => t('Filter messages depending on their deletion by the participant.')
    ),
  );

  // pm_block_user module
  if (module_exists('pm_block_user')) {
    $data['pm_block_user']['table']['group']  = t('Privatemsg');
    $data['pm_block_user']['table']['join'] = array(
      'pm_message' => array(
        'left_table' => 'pm_index',
        'left_field' => 'uid',
        'field' => 'recipient',
        'handler' => 'views_multiple_field_join',
        'extra' => array(
          'author' => array(
            'table' => 'pm_message',
            'field' => 'author',
            'operator' => '=',
          ),
        ),
      ),
    );
    $data['pm_block_user']['is_blocked'] = array(
      'title' => t('Blocked'),
      'help' => t('Whether the message is blocked by recipient.'),
      'real field' => 'recipient',
      'filter' => array(
        'handler' => 'views_handler_filter_boolean_operator',
        'type' => 'yes-no',
        'label' => t('Blocked'),
        // Workaround for a Views bug until http://drupal.org/node/508124#comment-2554662
        // gets into an official release.
        'accept null' => TRUE,
        'accept_null' => TRUE,
      ),
      'field' => array(
        'handler' => 'privatemsg_views_handler_field_is_blocked',
      ),
    );    
  }
}

/**
 * Implementation of hook_views_handlers().
 */
function privatemsg_views_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'privatemsg_views') . '/handlers',
    ),
    'handlers' => array(
      // Aggregation handlers
      'privatemsg_views_handler_field_thread_msg' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_updated' => array(
        'parent' => 'views_handler_field_date',
      ),
      'privatemsg_views_handler_field_thread_msg_body' => array(
        'parent' => 'views_handler_field',
      ),
      
      // Normal handlers
      'privatemsg_views_handler_field_unread_pagenum' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_is_new' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_is_blocked' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_message_actions' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_field_msg_count' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'privatemsg_views_handler_field_participants' => array(
        'parent' => 'views_handler_field',
      ),
      'privatemsg_views_handler_filter_participant' => array(
        'parent' => 'views_handler_filter',
      ),
    ),
  );
}

// Manually include custom join handler cause it's not included by Views unlike 
// normal handlers.
module_load_include('inc', 'privatemsg_views', 'handlers/views_multiple_field_join');
