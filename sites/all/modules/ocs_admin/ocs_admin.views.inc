<?php

/*
    Implementation of hook_views_data
*/
function ocs_admin_views_data() {

    $data = array();

    // OCS Customer Table
    $data['ocs_customer']['table']['name'] = 'ocsdata.ocs_customer';
    $data['ocs_customer']['table']['group'] = t('OCS Customer');
    $data['ocs_customer']['table']['base'] = array(
        'field' => 'customer_id',
        'title' => t('OCS Customer Table'),
        'help' => t('View records of OCS Cusomter Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_customer']['customer_id'] = array(
        'title' => t('Customer ID'),
        'help' => t('Customer\' ID'),
        'field' => array(
            'handler' => 'views_handler_field_numeric',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['ocs_customer']['customer_name'] = array(
        'title' => t('Customer Name'),
        'help' => t('Customer Name'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );


    // OCS Account
    $data['ocs_account']['table']['name'] = 'ocsdata.ocs_account';
    $data['ocs_account']['table']['group'] = t('OCS Account');
    $data['ocs_account']['table']['base'] = array(
        'field' => 'account_key',
        'title' => t('OCS Account Table'),
        'help' => t('View records of OCS Account Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );
    $data['ocs_account']['table']['join'] = array(
        'service_type' => array(
            'left_field' => 'service_id',
            'field' => 'service_id',
        ),
        'customer' => array(
            'left_field' => 'customer_id',
            'field' => 'customer_id'
        )
    );


    $data['ocs_account']['account_key'] = array(
        'title' => t('Account Key'),
        'help' => t('Account Key value such as MSISDN, User Name and etc'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    $data['ocs_account']['service_id'] = array(
        'title' => t('Service Type ID'),
        'help' => t('Service Type ID such as Mobile, IPTV, VoIP and etc'),
        'field' => array(
            'handler' => 'views_handler_field_numeric',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['ocs_account']['service_type'] = array(
        'title' => t('Service Type '),
        'help' => t('Service Type such as Mobile, IPTV, VoIP and etc'),
        'relationship' => array(
            'base' => 'service_type',
            'field' => 'service_id',
            'handler' => 'views_handler_relationship',
            'label' => t('Service Type')
        ),
    );

    $data['ocs_account']['customer'] = array(
        'title' => t('Customer '),
        'help' => t('Customer Information for this account'),
        'relationship' => array(
            'base' => 'ocs_customer',
            'field' => 'customer_id',
            'handler' => 'views_handler_relationship',
            'label' => t('Customer'),
        ),
    );


    // Service Type: Mobile, IPTV and etc
    $data['service_type']['table']['name'] = 'ocsdata.service_type';
    $data['service_type']['table']['group'] = t('Service Type');
    $data['service_type']['table']['base'] = array(
        'field' => 'service_id',
        'title' => t('Service Type'),
        'help' => t('Service Type such as Mobile, IPTV, VoIP and Broadband'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['service_type']['service_id'] = array(
        'title' => t('Service ID'),
        'help' => t('Service Type ID'),
        'field' => array(
            'handler' => 'views_handler_field_numeric',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['service_type']['service_name'] = array(
        'title' => t('Service Name'),
        'help' => t('Service Name'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    // Customer Table edit by charles 2012-03-29
    // Start
    $data['customer']['table']['name'] = 'ocsdata.customer';
    $data['customer']['table']['group'] = t('Customer');
    $data['customer']['table']['base'] = array(
        'field' => 'customerid',
        'title' => t('Customer Table'),
        'help' => t('View records of Cusomter Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['customer']['customerid'] = array(
        'title' => t('Customer ID'),
        'help' => t('Customer unique ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['customer']['firstname'] = array(
        'title' => t('First Name'),
        'help' => t('Customer First Name'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['customer']['lastname'] = array(
        'title' => t('Last Name'),
        'help' => t('Customer Last Name'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );
    
    $data['customer']['status'] = array(
        'title' => t('Status'),
        'help' => t('Customer Status'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    // Customer Table edit by charles 2012-03-29
    // Start
    $data['test_view']['table']['name'] = 'ocsdata.test_view';
    $data['test_view']['table']['group'] = t('Customer');
    $data['test_view']['table']['base'] = array(
        'field' => 'customer_id',
        'title' => t('Customer View'),
        'help' => t('View records of Cusomter Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['test_view']['customer_id'] = array(
        'title' => t('Customer ID'),
        'help' => t('Customer unique ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    // Product Table
    $data['product']['table']['name'] = 'ocsdata.product';
    $data['product']['table']['group'] = t('OCS Product');
    $data['product']['table']['base'] = array(
        'field' => 'product_id',
        'title' => t('OCS Product'),
        'help' => t('Product List registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['product']['product_id'] = array(
        'title' => t('Product ID'),
        'help' => t('Product ID'),
        'field' => array(
            'handler' => 'views_handler_field_numeric',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['product']['product_name'] = array(
        'title' => t('Product Name'),
        'help' => t('Product Name'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    $data['product']['description'] = array(
        'title' => t('Product Description'),
        'help' => t('Product Description'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    $data['product']['views_bulk_operations'] = array(
        'title' => t('Product checkbox'),
        'help' => t('Product checkbox'),
        'real field' => 'product_id',
        'field' => array(
            'handler' => 'views_bulk_operations_handler_field_operations',
            'click sortable' => FALSE,
        ),
    );

    // Customer Table edit by charles 2012-03-29
    // Start
    $data['test_view']['table']['name'] = 'ocsdata.test_view';
    $data['test_view']['table']['group'] = t('Customer');
    $data['test_view']['table']['base'] = array(
        'field' => 'customer_id',
        'title' => t('Customer View'),
        'help' => t('View records of Cusomter Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['test_view']['customer_id'] = array(
        'title' => t('Customer ID'),
        'help' => t('Customer unique ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    return $data;
}