<?php

/*
    Implementation of hook_views_data
*/
function ocs_admin_views_data() {

    $data = array();

    $customer = ocs_admin_table_customer();
    $data = array_merge( $data, $customer);

    $account = ocs_admin_table_account();
    $data = array_merge( $data, $account);
    
    $service = ocs_admin_table_service_type();
    $data = array_merge( $data, $service);

    $customer_view = ocs_admin_table_customer_view();
    $data = array_merge( $data, $customer_view);

    $product = ocs_admin_table_product();
    $data = array_merge( $data, $product);

    $account_key = ocs_admin_table_account_key_view();
    $data = array_merge( $data, $account_key);

    $group = ocs_admin_table_group();
    $data = array_merge( $data, $group);

    $member = ocs_admin_table_group_member();
    $data = array_merge( $data, $member);

//    dpm( $data, 'view_data');

    return $data;
}    

function ocs_admin_table_customer()
{
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

    return $data;
}    

function ocs_admin_table_account()
{
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


    $data['ocs_account']['customer_id'] = array(
        'title' => t('Customer ID'),
        'help' => t('Customer ID of this account'),
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

    return $data;
}    


function ocs_admin_table_service_type()
{

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

    return $data;
}    


function ocs_admin_table_customer_view()
{
// ocs_customer_view Table edit by charles 2012-04-04
    // Start
    $data['ocs_customer_view']['table']['name'] = 'ocsdata.ocs_customer_view';
    $data['ocs_customer_view']['table']['group'] = t('Customer');
    $data['ocs_customer_view']['table']['base'] = array(
        'field' => 'customer_id',
        'title' => t('Customer View'),
        'help' => t('View records of Cusomter Table'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_customer_view']['customer_id'] = array(
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

    $data['ocs_customer_view']['customer_name'] = array(
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
    );

    $data['ocs_customer_view']['code_name'] = array(
        'title' => t('Customer Type'),
        'help' => t('Customer Type (ocs_code_master : B302)'),
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

    $data['ocs_customer_view']['date_register'] = array(
        'title' => t('Register Date'),
        'help' => t('Register Date'),
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


function ocs_admin_table_product()
{
    $data['ocs_product_view']['table']['name'] = 'ocsdata.ocs_product';
    $data['ocs_product_view']['table']['group'] = t('OCS Product');
    $data['ocs_product_view']['table']['base'] = array(
        'field' => 'product_name',
        'title' => t('OCS Product'),
        'help' => t('Product List registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_product_view']['product_serial_no'] = array(
        'title' => t('Product Serial Number'),
        'help' => t('Product Serial No'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    $data['ocs_product_view']['product_name'] = array(
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

    $data['ocs_product_view']['product_desc'] = array(
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
            'handler' => 'views_handler_sort_string',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );

    $data['ocs_product_view']['product_status'] = array(
        'title' => t('Product Status'),
        'help' => t('Product Status'),
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

    $data['ocs_product_view']['product_period'] = array(
        'title' => t('Product Period'),
        'help' => t('Product Period'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_date',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort_date',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_date',
        ),
    );

    $data['ocs_product_view']['code_name'] = array(
        'title' => t('Code Name'),
        'help' => t('Code Name(Product Status : P101)'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_string',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort_string',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_string',
        ),
    );
    
    return $data;
    
    
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

    return $data;
}

function ocs_admin_table_account_key_view()
{

    // ocs_account_key_view edit by charles 2012-04-04
    // Start ocs_account_key_view
    $data['ocs_account_key_view']['table']['name'] = 'ocsdata.ocs_account_key_view';
    $data['ocs_account_key_view']['table']['group'] = t('OCS Account Key View');
    $data['ocs_account_key_view']['table']['base'] = array(
        'field' => 'account_key',
        'title' => t('OCS Account Key'),
        'help' => t('Account Resource List'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_account_key_view']['account_key'] = array(
        'title' => t('OCS Account key'),
        'help' => t('OCS Account key'),
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

    $data['ocs_account_key_view']['account_key_type'] = array(
        'title' => t('OCS Account Key Type'),
        'help' => t('OCS Account Key Type'),
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

    $data['ocs_account_key_view']['account_key_name'] = array(
        'title' => t('OCS Account Key Type Name'),
        'help' => t('OCS Account Key Type Name'),
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

    $data['ocs_account_key_view']['sales_flag'] = array(
        'title' => t('OCS Account Key Status'),
        'help' => t('OCS Account Key Status'),
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
    // End ocs_account_key_view

    return $data;
}

function ocs_admin_table_group()
{
    // Customer Group table
    $data['ocs_group']['table']['name'] = 'ocsdata.ocs_group';
    $data['ocs_group']['table']['group'] = t('OCS Customer Group');
    $data['ocs_group']['table']['base'] = array(
        'field' => 'group_id',
        'title' => t('OCS Customer Group'),
        'help' => t('Customer Groups registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_group']['group_id'] = array(
        'title' => t('Group ID'),
        'help' => t('Group ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['ocs_group']['parent_group_id'] = array(
        'title' => t('Parent Group ID'),
        'help' => t('Parent Group ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    $data['ocs_group']['group_name'] = array(
        'title' => t('Group Name'),
        'help' => t('Group Name'),
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

    $data['ocs_group']['master_number'] = array(
        'title' => t('Master Number'),
        'help' => t('Master Number'),
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

    return $data;
}

function ocs_admin_table_group_member()
{
    // Customer Group table
    $data['ocs_group_member']['table']['name'] = 'ocsdata.ocs_group_member';
    $data['ocs_group_member']['table']['group'] = t('OCS Group Members');
    $data['ocs_group_member']['table']['base'] = array(
        'field' => 'account_key',
        'title' => t('OCS Group Members'),
        'help' => t('Group Members registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_group_member']['table']['join'] = array(
        'group' => array(
            'left_field' => 'group_id',
            'field' => 'group_id'
        )
    );

    $data['ocs_group_member']['account_key'] = array(
        'title' => t('Account Key'),
        'help' => t('Account Key (MSISDN, User ID)'),
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

    $data['ocs_group_member']['group_id'] = array(
        'title' => t('Group ID'),
        'help' => t('Group ID'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_numeric',
        ),
    );

    $data['ocs_group_member']['extension'] = array(
        'title' => t('VPN extension'),
        'help' => t('VPN extension (3~4 digits)'),
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

    $data['ocs_group_member']['limitation'] = array(
        'title' => t('Usage Limitation'),
        'help' => t('Usage Limitation'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_numeric',
        ),
    );

    $data['ocs_group_member']['usage'] = array(
        'title' => t('Current Usage'),
        'help' => t('Current Usage'),
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_numeric',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_numeric',
        ),
    );

    $data['ocs_group_member']['group'] = array(
        'title' => t('Group'),
        'help' => t('Group that this account belongs to'),
        'relationship' => array(
            'base' => 'ocs_group',
            'field' => 'group_id',
            'handler' => 'views_handler_relationship',
            'label' => t('Group'),
        ),
    );

    return $data;
}
