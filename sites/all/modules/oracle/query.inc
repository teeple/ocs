<?php

/**
 * @ingroup database
 * @{
 */

class InsertQuery_oracle extends InsertQuery {
	
  public function __construct($connection, $table, array $options = array()) {
    parent::__construct($connection, $table, $options);
    $this->queryOptions['table_name']= $table;
    //syslog(LOG_ERR,"InsertQuery_oracle");
  }


  public function execute() {
    if (!$this->preExecute()) {
      return NULL;
    }

    $last_insert_id = 0;

    $info= $this->connection->schema()->getTableInfo($this->table);
    //syslog(LOG_ERR,"table: ".$this->table." sequence_name: ".$info->sequence_name);
    
    if (!empty($info->sequence_name)) {
      $this->queryOptions["sequence_name"] = $info->sequence_name;
      $this->queryOptions['return'] = Database::RETURN_INSERT_ID;
    }
    
    
    $transaction = $this->connection->startTransaction();
    $stmt = $this->connection->prepareQuery((string)$this)->getStatement(NULL);
    $last_insert_id= 0;
    
    foreach ($this->insertValues as &$insert_values)
    {
        $max_placeholder = 0;
        
    	foreach ($this->insertFields as $idx => $field)
        {
          $insert_values[$idx]= $this->connection->cleanupArgValue($insert_values[$idx]);
          $stmt->bindParam(':db_insert_placeholder_'. $max_placeholder++, $insert_values[$idx]);
        }
 
        $last_insert_id = $this->connection->query($stmt,  array(), $this->queryOptions);
    }

    if (!empty($this->fromQuery)) {
      foreach ($this->fromQuery->getArguments() as $key => $value) {
        $stmt->bindParam($key, $value);
      }
    }
    
    
    //syslog(LOG_ERR,"insert_values: ".print_r($insert_values,true));

    if (count($this->insertValues)==0) // all default query
        $last_insert_id = $this->connection->query($stmt,  array(), $this->queryOptions);
    
    //syslog(LOG_ERR,"inserted id: ".print_r($last_insert_id,true));
    
    // Re-initialize the values array so that we can re-use this query.
    $this->insertValues = array();

    // Transaction commits here where $transaction looses scope.

    return $last_insert_id;
    
  }

  public function __toString() {

    $info= $this->connection->schema()->getTableInfo($this->table);

    //syslog(LOG_ERR,"info: ".print_r($info,true));
    
  	// Default fields are always placed first for consistency.
    $insert_fields = array_merge($this->defaultFields, $this->insertFields);
    
    if (!empty($this->fromQuery))
    { 
      $cols= implode(', ', $insert_fields);
      
      if (!empty($cols))
        return "INSERT INTO {" . $this->table . '} (' . $cols . ') ' . $this->fromQuery;
      else
        return "INSERT INTO {" . $this->table . '}  ' . $this->fromQuery;
    }

    $query = "INSERT INTO {" . $this->table . '} (' . implode(', ', $insert_fields) . ') VALUES ';

    $max_placeholder = 0;
    $values = array();
    
    if (count($this->insertValues))
    {
      	$placeholders = array();
        $placeholders = array_pad($placeholders, count($this->defaultFields), 'default');
        $i= 0;
        foreach ($this->insertFields as $key => $value) 
           $placeholders[] = ':db_insert_placeholder_' . $i++;
        
      
        $values= '(' . implode(', ', $placeholders) . ')';
        
    }
    else
    {
	    if (count($this->defaultFields)>0)
	    {
	        // If there are no values, then this is a default-only query. We still need to handle that.
	        $placeholders = array_fill(0, count($this->defaultFields), 'default');
	        $values = '(' . implode(', ', $placeholders) .')';
	    }
	    else
	      $values = '()'; // meaningless query that will not be executed
    }

    $query .= $values;
    
    //syslog(LOG_ERR,"insert_string: ".$query." insertValues: ".print_r($this->insertValues,true));
    
    return $query;
  }
  
}


class UpdateQuery_oracle extends UpdateQuery {

  private $updateArgs= array();
  private $conditionArgs= array();
  private $expArgs= array();
  
  private $rowCount= 0;

  public function execute() {
    $max_placeholder = 0;
    $blobs = array();
    $blob_count = 0;
    
    // needed for BLOBS
    $transaction = $this->connection->startTransaction();

    // Because we filter $fields the same way here and in __toString(), the
    // placeholders will all match up properly.
    $stmt = $this->connection->prepareQuery((string)$this)->getStatement(NULL);

    // Fetch the list of blobs and sequences used on that table.
    $info= $this->connection->schema()->getTableInfo($this->table);

    // Expressions take priority over literal fields, so we process those first
    // and remove any literal fields that conflict.
    $fields = $this->fields;
    $expression_fields = array();
    foreach ($this->expressionFields as $field => $data) {
      if (!empty($data['arguments'])) {
        foreach ($data['arguments'] as $placeholder => $argument) {
          // We assume that an expression will never happen on a BLOB field,
          // which is a fairly safe assumption to make since in most cases
          // it would be an invalid query anyway.
          $this->expArgs[$placeholder]= $this->connection->cleanupArgValue($argument);
          $stmt->bindParam($placeholder, $this->expArgs[$placeholder]);
        }
      }
      unset($fields[$field]);
    }

    foreach ($fields as $field => $value)
    {
        $placeholder = ':db_update_placeholder_' . ($max_placeholder++);
        
        $this->updateArgs[$placeholder]= $this->connection->cleanupArgValue($value);
        
        $stmt->bindParam($placeholder, $this->updateArgs[$placeholder]);
    }

    if (count($this->condition)) {
      $this->condition->compile($this->connection, $this);

      $arguments = $this->condition->arguments();
      foreach ($arguments as $placeholder => $value) {
        $this->conditionArgs[$placeholder]= $this->connection->cleanupArgValue($value);
        $stmt->bindParam($placeholder, $this->conditionArgs[$placeholder]);
      }
    }

    //syslog(LOG_ERR,'conditionArgs: '.print_r($this->conditionArgs,true));
    //syslog(LOG_ERR,'updateArgs: '.print_r($this->updateArgs,true));
    //syslog(LOG_ERR,'expArgs: '.print_r($this->expArgs,true));

    $stmt->bindParam('db_sql_rowcount', $this->rowCount, PDO::PARAM_INT|PDO::PARAM_INPUT_OUTPUT, 32);
    
    $options = $this->queryOptions;
    $options['already_prepared'] = TRUE;
    $this->connection->query($stmt, array(), $options);
    ////syslog(LOG_ERR,"updateFields: ".print_r($this->fields,true));
    
    return (int)$this->rowCount;
  }
  
  public function __toString() 
  {

    // Create a comments string to prepend to the query.
    $comments = (!empty($this->comments)) ? '/* ' . implode('; ', $this->comments) . ' */ ' : '';

    // Expressions take priority over literal fields, so we process those first
    // and remove any literal fields that conflict.
    $fields = $this->fields;
    $update_fields = array();
    foreach ($this->expressionFields as $field => $data) {
      $update_fields[] = $field . '=' . $data['expression'];
      unset($fields[$field]);
    }

    $max_placeholder = 0;
    foreach ($fields as $field => $value) {
      $update_fields[] = $field . '=:db_update_placeholder_' . ($max_placeholder++);
    }

    $query = $comments . 'UPDATE {' . $this->connection->escapeTable($this->table) . '} SET ' . implode(', ', $update_fields);

    if (count($this->condition)) {
      $this->condition->compile($this->connection, $this);
      // There is an implicit string cast on $this->condition.
      $query .= "\nWHERE " . $this->condition;
    }

    // FIXME: remove the PL/SQL when PDO_OCI returns the correct number of
    // rows updated now it returns only 1 (updated) or 0 (not)
    return 'begin '.$query.'; :db_sql_rowcount := sql%rowcount; end;';
  }
  
}


class TruncateQuery_oracle extends TruncateQuery {

  public function __toString() {
    return 'TRUNCATE TABLE {' . $this->connection->escapeTable($this->table) . '} ';
  }
}

/**
 * @} End of "ingroup database".
 */
