<?php

function tvchat_views_data() {

   // just for test
  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  $data['tvchat_ranking_daily']['table']['group'] = t('Ranking Daily');

  // This table references the {node} table.
  // This creates an 'implicit' relationship to the node table, so that when 'Node'
  // is the base table, the fields are automatically available.
  $data['tvchat_ranking_daily']['table']['join'] = array(
    // Index this array by the table name to which this table refers.
    // 'left_field' is the primary key in the referenced table.
    // 'field' is the foreign key in this table.
    'node' => array(
      'type'  => 'INNER',
      'left_field' => 'nid',
      'field' => 'entity_nid',
    ),    
    'users' => array(
      'type'  => 'INNER',
      'left_field' => 'uid',
      'field' => 'entity_uid',
    ),    
  );

  $data['tvchat_ranking_daily']['entity_type'] = array(
    'title' => t('Type'),
    'help' => t('Type of record'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['tvchat_ranking_daily']['entity_nid'] = array(
    'title' => t('node'),
    'help' => t('nid of type'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),    
    'relationship' => array(
      'base'  => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node Id '),
    ),
  );

  $data['tvchat_ranking_daily']['entity_uid'] = array(
    'title' => t('user'),
    'help' => t('uid '),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base'  => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User Id '),
    ),
  );

  $data['tvchat_ranking_daily']['entity_taxonomy'] = array(
    'title' => t('genre - taxonomy'),
    'help' => t('genre of nid'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ), 
    'relationship' => array(
      'base'  => 'term_data',
      'base field' => 'tid',
      'handler' => 'views_handler_relationship',
      'label' => t('Taxonomy ID (term_data) '),
    ),
  );

  $data['tvchat_ranking_daily']['points'] = array(
    'title' => t('points'),
    'help' => t('points'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort'  => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['tvchat_ranking_daily']['target_date'] = array(
    'title' => t('target_date'),
    'help' => t('target_date'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['tvchat_ranking_daily']['timestamp'] = array(
    'title' => t('timestamp'),
    'help' => t("timestamp"),
    'field' => array(
        'handler' => 'views_handler_field_date',
    ),
    'sort' => array(
        'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
        'handler' => 'views_handler_filter_date',
    ),
   );

  // The 'group' index will be used as a prefix in the UI for any of this
  // table's fields, sort criteria, etc. so it's easy to tell where they came
  // from.
  $data['tvchat_ranking_weekly']['table']['group'] = t('Ranking Weekly');

  // This table references the {node} table.
  // This creates an 'implicit' relationship to the node table, so that when 'Node'
  // is the base table, the fields are automatically available.
  $data['tvchat_ranking_weekly']['table']['join'] = array(
    // Index this array by the table name to which this table refers.
    // 'left_field' is the primary key in the referenced table.
    // 'field' is the foreign key in this table.
    'node' => array(
      'type'  => 'INNER',
      'left_field' => 'nid',
      'field' => 'entity_nid',
    ),    
    'users' => array(
      'type'  => 'INNER',
      'left_field' => 'uid',
      'field' => 'entity_uid',
    ),    
  );

  $data['tvchat_ranking_weekly']['entity_type'] = array(
    'title' => t('Type'),
    'help' => t('Type of record'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['tvchat_ranking_weekly']['entity_nid'] = array(
    'title' => t('node'),
    'help' => t('nid of type'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base'  => 'node',
      'base field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node Id '),
    ),

  );

  $data['tvchat_ranking_weekly']['entity_uid'] = array(
    'title' => t('user'),
    'help' => t('uid '),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ), 
    'relationship' => array(
      'base'  => 'users',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User Id '),
    ),
  );

  $data['tvchat_ranking_weekly']['entity_taxonomy'] = array(
    'title' => t('genre - taxonomy'),
    'help' => t('genre of nid'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ), 
    'relationship' => array(
      'base'  => 'term_data',
      'base field' => 'tid',
      'handler' => 'views_handler_relationship',
      'label' => t('Taxonomy ID (term_data) '),
    ),
  );

  $data['tvchat_ranking_weekly']['points'] = array(
    'title' => t('points'),
    'help' => t('points'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort'  => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  $data['tvchat_ranking_weekly']['target_date'] = array(
    'title' => t('target_date'),
    'help' => t('target_date'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

   $data['tvchat_ranking_weekly']['timestamp'] = array(
    'title' => t('timestamp'),
    'help' => t("timestamp"),
    'field' => array(
        'handler' => 'views_handler_field_date',
    ),
    'sort' => array(
        'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
        'handler' => 'views_handler_filter_date',
    ),
   );


  return $data;
}

