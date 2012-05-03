<?php
//		error_reporting(E_ALL ^ E_NOTICE);
/*
    Implementation of hook_views_data
*/
function ocs_admin_views_data() {

    $data = array();

    $customer = ocs_admin_table_customer();
    $data = array_merge( $data, $customer);

    $account = ocs_admin_table_account();
    $data = array_merge( $data, $account);

    $info = ocs_admin_table_subscription();
    $data = array_merge( $data, $info);

    $info = ocs_admin_table_counter_name();
    $data = array_merge( $data, $info);
    
    $info = ocs_admin_table_counter();
    $data = array_merge( $data, $info);
    
    $service = ocs_admin_table_service_type();
    $data = array_merge( $data, $service);

    $customer_view = ocs_admin_table_customer_view();
    $data = array_merge( $data, $customer_view);

    $product_view = ocs_admin_table_product_view();
    $data = array_merge( $data, $product_view);

    $account_key = ocs_admin_table_account_key_view();
    $data = array_merge( $data, $account_key);

    $group = ocs_admin_table_group();
    $data = array_merge( $data, $group);

    $member = ocs_admin_table_group_member();
    $data = array_merge( $data, $member);

    $ocs_card_type = ocs_admin_card_type();
    $data = array_merge( $data, $ocs_card_type);

    $ocs_product_card = ocs_admin_product_card();
    $data = array_merge( $data, $ocs_product_card);

    $ocs_product = ocs_admin_product();
    $data = array_merge( $data, $ocs_product);

    $ocs_card = ocs_admin_card();
    $data = array_merge( $data, $ocs_card);

    $campaign = ocs_admin_table_campaign();
    $data = array_merge( $data, $campaign);
//  dpm( $data, 'view_data');
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

    $data['ocs_customer']['customer_id'] = _get_numeric_field( array(
        'title' => t('Customer ID'),
        'help' => t('Customer\' ID'),
    ));

    $data['ocs_customer']['customer_name'] = _get_string_field( array(
        'title' => t('Customer Name'),
        'help' => t('Customer Name'),
    ));

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
        ),
        'subscription' => array(
            'left_field' => 'subscription_key',
            'field' => 'subscription_key',
        ),
        'counter' => array(
            'left_field' => 'subscription_key',
            'field' => 'subscription_key',
        ),
    );

    $data['ocs_account']['customer_id'] = _get_string_field( array(
        'title' => t('Customer ID'),
        'help' => t('Customer ID of this account'),
    ));


    $data['ocs_account']['account_key'] = _get_string_field( array(
        'title' => t('Account Key'),
        'help' => t('Account Key value such as MSISDN, User Name and etc'),
    ));

    $data['ocs_account']['status'] = _get_string_field( array(
        'title' => t('Status'),
        'help' => t('Account Key Status'),
    ));

    $data['ocs_account']['service_id'] = _get_numeric_field( array(
        'title' => t('Service Type ID'),
        'help' => t('Service Type ID such as Mobile, IPTV, VoIP and etc'),
    ));

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

    $data['ocs_account']['subscription_key'] = array(
        'title' => t('Subscription'),
        'help' => t('Subscription data for this account'),
        'relationship' => array(
            'base' => 'ocs_subscription',
            'field' => 'subscription_key',
            'handler' => 'views_handler_relationship',
            'label' => t('Subscription'),
        ),
    );

    $data['ocs_account']['counter'] = array(
        'title' => t('Counter'),
        'help' => t('Counter values for this account'),
        'relationship' => array(
            'base' => 'abmf_counter',
            'field' => 'subscription_key',
            'handler' => 'views_handler_relationship',
            'label' => t('Counter'),
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

    $data['service_type']['service_id'] = _get_numeric_field( array(
        'title' => t('Service ID'),
        'help' => t('Service Type ID'),
    ));

    $data['service_type']['service_name'] = _get_string_field( array(
        'title' => t('Service Name'),
        'help' => t('Service Name'),
    ));

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

    $data['ocs_customer_view']['customer_id'] = _get_string_field( array(
        'title' => t('Customer ID'),
        'help' => t('Customer unique ID'),
    ));

    $data['ocs_customer_view']['customer_name'] = _get_string_field( array(
        'title' => t('Customer Name'),
        'help' => t('Customer Name'),
    ));

    $data['ocs_customer_view']['code_name'] = _get_string_field( array(
        'title' => t('Customer Type'),
        'help' => t('Customer Type (ocs_code_master : B302)'),
    ));

    $data['ocs_customer_view']['sub_code'] = _get_numeric_field( array(
        'title' => t('Customer Type Code'),
        'help' => t('Customer Type Code (ocs_code_master : B302)'),
    ));

    $data['ocs_customer_view']['date_register'] = _get_string_field( array(
        'title' => t('Register Date'),
        'help' => t('Register Date'),
    ));

    return $data;
}    


function ocs_admin_table_product_view()
{
    $data['ocs_product_view']['table']['name'] = 'ocsdata.ocs_product';
    $data['ocs_product_view']['table']['group'] = t('OCS Product View');
    $data['ocs_product_view']['table']['base'] = array(
        'field' => 'product_name',
        'title' => t('OCS Product View'),
        'help' => t('Product List registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_product_view']['table']['join'] = array(
        'taxonomy_term_data' => array(
            'left_field' => 'tid',
            'field' => 'product_tid',
        ),
    );

    $data['ocs_product_view']['product_serial_no'] = _get_numeric_field( array(
        'title' => t('Product Serial Number'),
        'help' => t('Product Serial No'),
    ));

    $data['ocs_product_view']['product_name'] = _get_string_field( array(
        'title' => t('Product Name'),
        'help' => t('Product Name'),
    ));

    $data['ocs_product_view']['product_desc'] = _get_string_field( array(
        'title' => t('Product Description'),
        'help' => t('Product Description'),
    ));

    $data['ocs_product_view']['product_status'] = _get_string_field( array(
        'title' => t('Product Status'),
        'help' => t('Product Status'),
    ));

    $data['ocs_product_view']['product_period'] = _get_datetime_field( array(
        'title' => t('Product Period'),
        'help' => t('Product Period'),
    ));

    $data['ocs_product_view']['code_name'] = _get_string_field( array(
        'title' => t('Code Name'),
        'help' => t('Code Name(Product Status : P101)'),
    ));

   $data['ocs_product_view']['taxonomy_term_data'] = array(
        'title' => t('Taxonomy Term'),
        'help' => t('Taxonomy Term for Product Type'),
        'relationship' => array(
            'base' => 'taxonomy_term_data',
            'field' => 'product_tid',
            'handler' => 'views_handler_relationship',
            'label' => t('Product Type Term'),
            'database' => 'default',
        ),
    );

 
    return $data;
    
/*    
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
*/
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

    $data['ocs_account_key_view']['account_key'] = _get_numeric_field( array(
        'title' => t('OCS Account key'),
        'help' => t('OCS Account key (MSISDN)'),
    ));

    $data['ocs_account_key_view']['account_key_type'] = _get_string_field( array(
        'title' => t('OCS Account Key Type'),
        'help' => t('Account Key Type: Gold, Silver and Bronze'),
    ));

    $data['ocs_account_key_view']['account_key_name'] = _get_string_field( array(
        'title' => t('OCS Account Key Type Name'),
        'help' => t('Account Key Type Name : Gold, Silver and Bronze'),
    ));

    $data['ocs_account_key_view']['sales_flag'] = _get_string_field( array(
        'title' => t('OCS Account Key Status'),
        'help' => t('Account Key Status: Used, Not Used'),
    ));
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

    $data['ocs_group']['group_id'] = _get_numeric_field( array(
        'title' => t('Group ID'),
        'help' => t('Group ID'),
    ));

    $data['ocs_group']['parent_group_id'] = _get_numeric_field( array(
        'title' => t('Parent Group ID'),
        'help' => t('Parent Group ID'),
    ));

    $data['ocs_group']['ancestor_group_id'] = _get_numeric_field( array(
        'title' => t('Ancestor Group ID'),
        'help' => t('Ancestor Group ID'),
    ));

    $data['ocs_group']['group_name'] = _get_string_field( array(
        'title' => t('Group Name'),
        'help' => t('Group Name'),
    ));

    $data['ocs_group']['master_number'] = _get_string_field( array(
        'title' => t('Master Number'),
        'help' => t('Master Number'),
    ));

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

    $data['ocs_group_member']['account_key'] = _get_string_field( array(
        'title' => t('Account Key'),
        'help' => t('Account Key (MSISDN, User ID)'),
    ));

    $data['ocs_group_member']['group_id'] = _get_numeric_field( array(
        'title' => t('Group ID'),
        'help' => t('Group ID'),
    ));

    $data['ocs_group_member']['extension'] = _get_string_field( array(
        'title' => t('VPN extension'),
        'help' => t('VPN extension (3~4 digits)'),
    ));

    $data['ocs_group_member']['limitation'] = _get_numeric_field( array(
        'title' => t('Usage Limitation'),
        'help' => t('Usage Limitation'),
    ));

    $data['ocs_group_member']['account_usage'] = _get_numeric_field( array(
        'title' => t('Current Usage'),
        'help' => t('Current Usage'),
    ));

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

function ocs_admin_card_type()
{
    // Card Type
    $data['ocs_card_type']['table']['name'] = 'ocsdata.ocs_card_type';
    $data['ocs_card_type']['table']['group'] = t('OCS Card Type');
    $data['ocs_card_type']['table']['base'] = array(
        'field' => 'card_type',
        'title' => t('OCS Card Type'),
        'help' => t('OCS Card Type'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_card_type']['card_type'] = _get_string_field( array(
        'title' => t('Card Type'),
        'help' => t('Card Type'),
    ));

    $data['ocs_card_type']['unit'] = _get_numeric_field( array(
        'title' => t('Unit'),
        'help' => t('Card unit'),
    ));

    $data['ocs_card_type']['active_period'] = _get_numeric_field( array(
        'title' => t('Active Period'),
        'help' => t('Active Period'),
    ));

    $data['ocs_card_type']['grace_period'] = _get_numeric_field( array(
        'title' => t('Grace Period'),
        'help' => t('Grace Period'),
    ));

    $data['ocs_card_type']['description'] = _get_string_field( array(
        'title' => t('Description'),
        'help' => t('Description'),
    ));

    return $data;
}

function ocs_admin_product() 
{
    $data['ocs_product']['table']['name'] = 'ocsdata.ocs_product';
    $data['ocs_product']['table']['group'] = t('OCS Product');
    $data['ocs_product']['table']['base'] = array(
        'field' => 'product_serial_no',
        'title' => t('OCS Product'),
        'help' => t('Product List registered in OCS'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_product']['product_serial_no'] = _get_string_field( array(
        'title' => t('Product ID'),
        'help' => t('Product ID'),
    ));

    $data['ocs_product']['product_name'] = _get_string_field( array(
        'title' => t('Product Name'),
        'help' => t('Product Name'),
    ));

    $data['ocs_product']['description'] = _get_string_field( array(
        'title' => t('Product Description'),
        'help' => t('Product Description'),
    ));

    $data['ocs_product']['serial'] = array(
        'title' => t('Product ID'),
        'help' => t('Product ID'),
        'relationship' => array(
            'base' => 'ocs_product_card',
            'field' => 'product_serial_no',
            'handler' => 'views_handler_relationship',
            'label' => t('Product ID'),
        ),
    );

    return $data;
}

function ocs_admin_product_card()
{
    // Relate Product ID
    $data['ocs_product_card']['table']['name'] = 'ocsdata.ocs_product_card';
    $data['ocs_product_card']['table']['group'] = t('OCS Product Card');
    $data['ocs_product_card']['table']['base'] = array(
        'field' => 'card_type',
        'title' => t('OCS Product Card'),
        'help' => t('OCS Product Card'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_product_card']['card_type'] = _get_string_field( array(
        'title' => t('Card Type'),
        'help' => t('Card Type'),
    ));


    $data['ocs_product_card']['product_id'] = _get_string_field( array(
        'title' => t('Product ID'),
        'help' => t('Product ID'),
    ));

    $data['ocs_product_card']['usable'] = _get_enum_field( 'usable_type', array(
        'title' => t('Usable'),
        'help' => t('Usable'),
    ));

    $data['ocs_product_card']['product_name'] = array(
        'title' => t('Product Card Serial No'),
        'help' => t('Product Card Serial No'),
        'relationship' => array(
            'base' => 'ocs_product',
            'field' => 'product_id',
            'handler' => 'views_handler_relationship',
            'label' => t('Product ID'),
        ),
    );
    

    return $data;
}

function ocs_admin_card() 
{
    // Card
    $data['ocs_card']['table']['name'] = 'ocsdata.ocs_card';
    $data['ocs_card']['table']['group'] = t('OCS Card');
    $data['ocs_card']['table']['base'] = array(
        'field' => 'card_no',
        'title' => t('OCS Card'),
        'help' => t('OCS Card'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_card']['card_no'] = _get_string_field( array(
        'title' => t('Card No'),
        'help' => t('Card No'),
    ));


    $data['ocs_card']['card_type'] = _get_string_field( array(
        'title' => t('Card Type'),
        'help' => t('Card Type'),
    ));

    $data['ocs_card']['pin_no'] = _get_string_field( array(
        'title' => t('PIN Number'),
        'help' => t('PIN Number'),
    ));

    $data['ocs_card']['recharge_datetime'] = _get_datetime_field( array(
        'title' => t('Recharge DateTime'),
        'help' => t('Recharge DateTime'),
    ));


    $data['ocs_card']['account_key'] = _get_string_field( array(
        'title' => t('Account Key'),
        'help' => t('Account Key'),
    ));

    $data['ocs_card']['status'] = _get_enum_field( 'card_status', array(
        'title' => t('Status'),
        'help' => t('Satus'),
    ));

    /*
    $data['ocs_card']['status'] = _get_string_field( array(
        'title' => t('Status'),
        'help' => t('Satus'),
    ));
    */
    return $data;
}

function ocs_admin_table_subscription() 
{
    // ocs_subscription
    $data['ocs_subscription']['table']['name'] = 'ocsdata.ocs_subscription';
    $data['ocs_subscription']['table']['group'] = t('OCS Subscription');
    $data['ocs_subscription']['table']['base'] = array(
        'field' => 'subscription_key',
        'title' => t('OCS Subscription'),
        'help' => t('Account Subscription'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_subscription']['subscription_key'] = _get_numeric_field( array(
        'title' => t('Subscription Key'),
        'help' => t('Key for subscription table'),
    ));


    $data['ocs_subscription']['subscription_type'] = _get_enum_field( 'subscription_type', array(
        'title' => t('Subscription Type'),
        'help' => t('Subscription Type'),
    ));

    $data['ocs_subscription']['charging_type'] = _get_enum_field( 'charging_type', array(
        'title' => t('Charging Type STR'),
        'help' => t('Charging Type STR'),
    ));

    $data['ocs_subscription']['status'] = _get_numeric_field( array(
        'title' => t('Subscription Status'),
        'help' => t('Subscription Status'),
    ));

    $data['ocs_subscription']['active_expire_date'] = _get_datetime_field( array(
        'title' => t('Active Status Expiration DateTime'),
        'help' => t('Active Status Expiration DateTime'),
    ));

    $data['ocs_subscription']['grace_expire_date'] = _get_datetime_field( array(
        'title' => t('Grace Status Expiration DateTime'),
        'help' => t('Grace Status Expiration DateTime'),
    ));


    $data['ocs_subscription']['preferred_language'] = _get_enum_field( 'preferred_language', array(
        'title' => t('Preferred Language'),
        'help' => t('Preferred Language'),
    ));

    $data['ocs_subscription']['notification_flag_set'] = _get_enum_field( 'notification_flag_set', array(
        'title' => t('Notification Flag'),
        'help' => t('Notification Flag'),
    ));
    return $data;
	
}


function ocs_admin_table_counter_name() 
{
    // ocs_counter_name
    $data['countername_tbl']['table']['name'] = 'ocsdata.countername_tbl';
    $data['countername_tbl']['table']['group'] = t('OCS Counter Name');
    $data['countername_tbl']['table']['base'] = array(
        'field' => 'counter_id',
        'title' => t('OCS Counter Name'),
        'help' => t('Counter Name'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['countername_tbl']['counter_id'] = _get_numeric_field( array(
        'title' => t('Counter ID'),
        'help' => t('Counter ID'),
    ));


    $data['countername_tbl']['counter_name'] = _get_string_field( array(
        'title' => t('Counter Name'),
        'help' => t('Counter Name'),
    ));

    $data['countername_tbl']['counter_desc'] = _get_string_field( array(
        'title' => t('Counter Description'),
        'help' => t('Counter Description'),
    ));

    $data['countername_tbl']['counter_type'] = _get_numeric_field( array(
        'title' => t('Counter Type'),
        'help' => t('Counter Type'),
    ));

    $data['countername_tbl']['counter_scale'] = _get_numeric_field( array(
        'title' => t('Counter Scale'),
        'help' => t('Counter Scale'),
    ));

    return $data;
	
}


function _get_numeric_field( $info)
{
    $default_value = array(
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

    return array_merge( $default_value, $info);
}

function _get_datetime_field( $info)
{
    $default_value = array(
        'field' => array(
            'handler' => 'views_handler_field',
            'click sortable' => TRUE,
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_datetime',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
        'argument' => array( 
            'handler' => 'views_handler_argument_datetime',
        ),
    );

    return array_merge( $default_value, $info);
}

function _get_string_field( $info)
{
    $default_value = array(
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

    return array_merge( $default_value, $info);
}

function _get_enum_field( $type, $info)
{
    $default_value = array(
        'field' => array(
            'handler' => 'views_handler_field_' . $type,
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

    return array_merge( $default_value, $info);
}

function _get_status_field( $info)
{
    $default_value = array(
        'field' => array(
            'handler' => 'views_handler_field_boolean',
            'click sortable' => TRUE,
            'output formats' => array(
                'active-blocked' => array(t('Used'), t('Not Use')),
            ),
        ),
        'filter' => array( 
            'handler' => 'views_handler_filter_boolean_operator',
            'label' => t('Active'),
            'type' => 'yes-no',
        ),
        'sort' => array( 
            'handler' => 'views_handler_sort',
        ),
    );

    return array_merge( $default_value, $info);
}

function ocs_admin_table_counter() 
{
    // ocs_counter
    $data['abmf_counter']['table']['name'] = 'ocsdata.abmf_counter';
    $data['abmf_counter']['table']['group'] = t('OCS Counter');
    $data['abmf_counter']['table']['base'] = array(
        'field' => 'subscription_key',
        'title' => t('OCS Counter'),
        'help' => t('OCS Counter'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['abmf_counter']['table']['join'] = array(
        'counter_name' => array(
            'left_field' => 'counter_id',
            'field' => 'counter_id',
        ),
    );

    $data['abmf_counter']['subscription_key'] = _get_numeric_field( array( 
        'title'=> t('Subscription Key'), 
        'help' => t('Subscription Key')));
    $data['abmf_counter']['counter_id'] = _get_numeric_field( array(
        'title' => t('Counter ID'), 
        'help' => t('Counter ID')));
    $data['abmf_counter']['counter_value'] = _get_numeric_field( array(
        'title' => t('Counter Value'), 
        'help' => t('Counter Value')));
    $data['abmf_counter']['counter_time_stamp'] = _get_numeric_field(array(
        'title' => t('Counter Time Stamp'), 
        'help' => t('Counter Time Stamp')));
    $data['abmf_counter']['counter_expire'] = _get_numeric_field( array(
        'title' => t('Counter Expire'), 
        'help' => t('Counter Expire')));
    $data['abmf_counter']['counter_deposit'] = _get_numeric_field( array(
        'title' => t('Counter Deposit'), 
        'help' => t('Counter Deposit')));
    $data['abmf_counter']['deposit_time'] = _get_numeric_field( array(
        'title' => t('Deposit Time '), 
        'help' => t('Deposit Time')));
    $data['abmf_counter']['deposit_count'] = _get_numeric_field( array(
        'title' => t('Deposit Count'), 
        'help' => t('Deposit Count')));
    $data['abmf_counter']['counter_state'] = _get_numeric_field( array(
        'title' => t('Counter State'), 
        'help' => t('Counter State')));
    $data['abmf_counter']['counter_max_value'] = _get_numeric_field( array(
        'title' => t('Counter Max Value'), 
        'help' => t('Counter Max Value')));
    $data['abmf_counter']['counter_value2'] = _get_numeric_field( array(
        'title' => t('Counter Value 2'), 
        'help' => t('Counter Value 2')));

    $data['abmf_counter']['counter_name'] = array(
        'title' => t('Counter Name'),
        'help' => t('Counter Name'),
        'relationship' => array(
            'base' => 'countername_tbl',
            'field' => 'counter_id',
            'handler' => 'views_handler_relationship',
            'label' => t('Counter Name')
        ),
    );


    return $data;
	
}

function ocs_admin_table_campaign() 
{
    // ocs_counter
    $data['ocs_campaign']['table']['name'] = 'ocsdata.ocs_campaign';
    $data['ocs_campaign']['table']['group'] = t('OCS Campaign');
    $data['ocs_campaign']['table']['base'] = array(
        'field' => 'campaign_id',
        'title' => t('OCS Campaign'),
        'help' => t('List Campaigns'),
        'database' => 'ocsdb',
        'weight' => -10,
    );

    $data['ocs_campaign']['campaign_id'] = _get_numeric_field( array( 
        'title'=> t('Campaign ID'), 
        'help' => t('Campaign ID')));
    $data['ocs_campaign']['campaign_name'] = _get_string_field( array( 
        'title'=> t('Campaign Name'), 
        'help' => t('Campaign Name')));
    $data['ocs_campaign']['description'] = _get_string_field( array( 
        'title'=> t('Campaign Description'), 
        'help' => t('Campaign Description')));
    $data['ocs_campaign']['status'] = _get_numeric_field( array( 
        'title'=> t('Campaign Status'), 
        'help' => t('Campaign Status')));
    $data['ocs_campaign']['start_date'] = _get_datetime_field( array( 
        'title'=> t('Campaign Start Date'), 
        'help' => t('Campaign Start Date')));
    $data['ocs_campaign']['end_date'] = _get_datetime_field( array( 
        'title'=> t('Campaign End Date'), 
        'help' => t('Campaign End Date')));

    return $data;
	
}

// view handler
class views_handler_field_charging_type extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'charging_type');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

class views_handler_field_notification_flag_set extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'notification_flag_set');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

class views_handler_field_preferred_language extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'preferred_language');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

class views_handler_field_subscription_type extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'subscription_type');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

class views_handler_field_card_status extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'card_status');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

class views_handler_field_usable_type extends views_handler_field {
  
  function init(&$view, &$options) {
    parent::init($view, $options);
    if (empty($this->options['term_list'])) {
        $this->options['term_list'] = ocs_admin_get_term_list( 'usable_type');
    }
  }

  function render($values) {
    $value = $this->get_value($values);
    return ( isset( $this->options['term_list'][$value])) ? $this->options['term_list'][$value] : $value;
  }
}

