<?php    
/* all live-data in:    
   sites/all/modules/a_wusel_migration/data_sources/    
*/    
/* based on: drupal.org/node/1269066#comment-4988994 */    
/* and:      drupal.org/node/1190958#comment-4616032 */

abstract class CustomerMigration extends Migration {  
	public function __construct() {    
		// Always call the parent constructor first for basic setup    
		parent::__construct(MigrateGroup::getInstance('WuselMigrate'));  
	}
}

class Wusel_Step1_UserMigration extends CustomerMigration {  
	public function __construct() {    
		parent::__construct();    
		$this->description = t('Import an CSV-file (only "Account"-fields)');    
		$columns = array(      
			// "Source": ('Fieldname', 'Description')      
			0 => array('customerid', t('Customer ID (must be unique)')),      
			1 => array('firstname', t('Customer First Name')),      
			2 => array('lastname', t('Customer Last Name'))    
		);    
		// TIP: delete ", array('header_rows' => 1)" in the next line, 
		// when the CSV-file has NO header-line    
		$this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
										drupal_get_path('module', 'a_wusel_migration') . 
		  							'/data_sources/customer.csv', 
		  						  $columns, array('header_rows' => 0));    
		$this->destination = new MigrateDestinationUser();    

		$this->map = new MigrateSQLMap($this->machineName,        
			array('MID' => array( // this field is used to connect user und profile2
							'type' => 'varchar',                
							'length' => 6,                
							'not null' => TRUE,                
							'description' => t('User\'s Member-ID') // description never used 
							)             
						),        
			MigrateDestinationUser::getKeySchema()    
		);    
		
		
		// Mapped fields    
		$this->addSimpleMappings(array('name', 'mail'));    
		$this->addFieldMapping('pass', 'password')      
			->defaultValue('asdf');    
		$this->addFieldMapping('is_new')      
			->defaultValue(TRUE);    
		$this->addFieldMapping('roles')      
			->defaultValue(2); // 2 = "authenticated user"    
		$this->addFieldMapping('theme')      
			->defaultValue('');    
		$this->addFieldMapping('signature')      
			->defaultValue('');    
		$this->addFieldMapping('signature_format')      
			->defaultValue('filtered_html');    
		$this->addFieldMapping('created')      
			->defaultValue(time());    
		$this->addFieldMapping('access')      	
			->defaultValue(0);    
		$this->addFieldMapping('login')      
			->defaultValue(0);    
		$this->addFieldMapping('status')      
			->defaultValue(1);    
		$this->addFieldMapping('timezone')      
			->defaultValue(NULL);    
		$this->addFieldMapping('language')      
			->defaultValue(t('en')); // e.g.: 'en', 'fr', 'de', ...    
		$this->addFieldMapping('picture')      
			->defaultValue(0);    
		$this->addFieldMapping('init', 'mail');       
		
		// Other handlers    
		$this->addFieldMapping('path')      
			->defaultValue(NULL);    
		if (module_exists('pathauto')) {      
			$this->addFieldMapping('pathauto')        
				->defaultValue(''); // Perform aliasing (set to 0 to prevent alias generation during migration)    
		}
	}
}

class Wusel_Step2_MemberlistMigration extends CustomerMigration {  
	public function __construct() {    
		parent::__construct();    
		global $user;    
		$this->description = t('Import an CSV-file with Profile2-fields ("memberlist"-fields)');    
		$columns = array(      // "Source": ('Fieldname', 'Description')      
			0 => array('MID', t('Member-ID (must be unique)')),      
			1 => array('mail', t('Email (Account)')),      
			2 => array('name', t('Name (Account)')),      
			3 => array('password', t('Password (Account)')),      
			4 => array('complete_name', t('Complete Name (for Memberlist)')),      
			5 => array('birthday', t('Birthday (for Memberlist)')),      
			6 => array('tel_1', t('Tel.#1 (for Memberlist)'))    );    
			// TIP: delete ", array('header_rows' => 1)" in the next line, when the CSV-file has NO header-line    
			$this->source = new MigrateSourceCSV(DRUPAL_ROOT . '/' . 
			                drupal_get_path('module', 'a_wusel_migration') . 
			                '/data_sources/customer.csv', 
			                $columns, array('header_rows' => 1));    
			                // Declare migration 'Wusel_Step1_User' a dependency in migration 
			                // 'Wusel_Step2_Memberlist' to have them run in the right order, if needed:    
			$this->dependencies = array('Wusel_Step1_User');    
			$this->destination = new MigrateDestinationProfile2('memberlist');  
			// 'memberlist' is the "Machine name" of the profile2-"Profile type"    
			$this->map = new MigrateSQLMap($this->machineName,        
				array('MID' => array( // this field is used to connect user und profile2   
								'type' => 'varchar',                
								'length' => 6,                
								'not null' => TRUE,                
								'description' => t('User\'s Member-ID') // description never used
								)             
							),        
				MigrateDestinationProfile2::getKeySchema()    
			);    
			// Connecting the profile2 to the user:    
			
			$this->addFieldMapping('uid', 'MID')         
					 ->sourceMigration('Wusel_Step1_User')  // If your user migration class was named 'MyUserMigration', the string is 'MyUser'         
					 ->description(t('The assignment of profile2-items to the respective user.')); /* */    
					 
			// Mapped fields    
			$this->addFieldMapping('field_mnr', 'MID')      
				->defaultValue(0)      
				->description(t('The Member-ID (must be unique).'));    
			$this->addFieldMapping('field_username', 'name')      
				->defaultValue('')      
				->description(t('The login name.'));    
			$this->addFieldMapping('field_name', 'complete_name')      
				->defaultValue('')      
				->description(t('The complete name (for Memberlist).'));    
			$this->addFieldMapping('field_birthday', 'birthday')      
				->defaultValue(0)      
				->description(t('The birthday.') . '<br />' . t('Format') . ': "YYYY-MM-DD".');    
			$this->addFieldMapping('field_tel_1', 'tel_1')      
				->defaultValue('')      
				->description(t('The main telephone-number.'));    
			// some internal fields    
			$this->addFieldMapping('revision_uid')      
				->defaultValue($user->uid); // The user ID of the user, who started the migration    
			$this->addFieldMapping('language')      
				->defaultValue(t('en')); // e.g.: 'en', 'fr', 'de', ...    
			$this->addFieldMapping('path')      
				->defaultValue('');    
			
			// Unmapped fields (this fields are in core and not needed as profile2-fields)    
			$this->addUnmigratedSources(array('mail', 'password'));  
		}
	}
	?>
		