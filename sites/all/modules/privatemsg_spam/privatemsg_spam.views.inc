<?php
/**
 * Implementation of hook_views_data()
 */

/*
function privatemsg_spam_views_data(){
	// ----------------------------------------------------------------
	// pm_spam table (SPAM List)

	$data['pm_spam']['table']['group'] = t('SPAM List');

	$data['pm_spam']['table']['base'] = array(
	  'field' => 'word',
	  'title' => t('SPAM Word'),
	  'help' => t('SPAM Word')
	);

    $data['pm_spam']['word'] = array(
	    'title' => t('word'),
	    'help' => t('SPAM Word'),
	    'field' => array(
	      'handler' => 'views_handler_field',
	    ),
	    'filter' => array(
	      'handler' => 'views_handler_filter_string',
	    ),
	    'sort' => array(
	      'handler' => 'views_handler_sort',
	    ),
	    'argument' => array(
	      'handler' => 'views_handler_argument_string',
	    )
	);

  return $data;
}
*/