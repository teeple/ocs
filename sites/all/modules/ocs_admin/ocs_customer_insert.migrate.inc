<?php    
/* all live-data in:    
   sites/all/modules/a_wusel_migration/data_sources/    
*/    
/* based on: drupal.org/node/1269066#comment-4988994 */    
/* and:      drupal.org/node/1190958#comment-4616032 */

	abstract class CustomerInsertMigration extends Migration {  
		public function __construct() {    
			// Always call the parent constructor first for basic setup    
			parent::__construct(MigrateGroup::getInstance('CustomerInsertMigrate'));  
		}
	}

	class CustomerInsertMigration extends CustomerInsertMigration {  
		public function __construct() {    
			parent::__construct();    
			$this->description = t('Import an CSV-file (only "Account"-fields)');
			$columns = array(      
				// "Source": ('Fieldname', 'Description')      
				0 => array('customerid', t('customerid (must be unique)')),      
				1 => array('firstname', t('Customer First Name')),      
				2 => array('lastname', t('Customer Last Name')),      
			);    
			// TIP: delete ", array('header_rows' => 1)" in the next line, 
			// when the CSV-file has NO header-line    
			$this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
			                drupal_get_path('module', 'a_wusel_migration') . 
			                '/data_sources/customer.csv', 
			                $columns, array('header_rows' => 1));    
			$this->destination = new MigrateDestinationUser();    
			$this->map = new MigrateSQLMap($this->machineName,        
				array('customerid' => array( // this field is used to connect user und profile2   
				                      'type' => 'varchar',                
				                      'length' => 14,                
				                      'not null' => FALSE,                
				                      'description' => t('Customer ID') // description never used          
				      								)             
							),        
				MigrateDestinationUser::getKeySchema()    
			);    
			
			// Mapped fields    
			$this->addSimpleMappings(array('firstname', 'firstname'));
			$this->addFieldMappings(array('lastname'))
				->defaultVale('');    
		}
	}
/*	
	class Wusel_Step2_MemberlistMigration extends ocs_customer_insert {  
		public function __construct() {    
			parent::__construct();    
			global $user;    
			$this->description = t('Import an CSV-file with Profile2-fields ("memberlist"-fields)');    
			$columns = array(      
				// "Source": ('Fieldname', 'Description')      
				0 => array('customerid', t('Customer ID (must be unique)')),      
				1 => array('firstname', t('First Name')),      
				1 => array('lasttname', t('Last Name')),      
				// TIP: delete ", array('header_rows' => 1)" in the next line, when the CSV-file has NO header-line    
				);
				$this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
				                                     drupal_get_path('module', 'a_wusel_migration') . 
				                                     '/data_sources/customer.csv', 
				                                     $columns, array('header_rows' => 1));    
				// Declare migration 'Wusel_Step1_User' a dependency in migration 'Wusel_Step2_Memberlist' to have them run in the right order, if needed:    
				$this->dependencies = array('Wusel_Step1_User');    
				$this->destination = new MigrateDestinationProfile2('memberlist'); // 'memberlist' is the "Machine name" of the profile2-"Profile type"    
				$this->map = new MigrateSQLMap($this->machineName,        
					array('customerid' => array( // this field is used to connect user und profile2 				'type' => 'varchar',                
									'length' => 14,                
									'not null' => FALSE,                
									'description' => t('Customer ID') // description never used)
									)
								),     
						MigrateDestinationProfile2::getKeySchema()    
					);    
					
					// Connecting the profile2 to the user:    
					$this->addFieldMapping('customerid', 'customerid')         
							 ->sourceMigration('Wusel_Step1_User')  
							 // If your user migration class was named 'MyUserMigration', the string is 'MyUser'         
							 ->description(t('The assignment of profile2-items to the respective user.')); 
							 
					// Mapped fields    
					$this->addFieldMapping('customerid', 'cutomerid')      
						->defaultValue(0)      
						->description(t('customerid (must be unique).'));    
					$this->addFieldMapping('firstname', 'firstname')      
						->defaultValue('')      	
						->description(t('First Name'));    
					$this->addFieldMapping('lastname', 'lastname')      
						->defaultValue('')      	
						->description(t('Last Name'));    
				}
		}

*/