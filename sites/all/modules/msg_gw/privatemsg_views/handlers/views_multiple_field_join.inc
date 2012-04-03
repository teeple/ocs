<?php

/**
 * @file
 * Custom join handler for the Privatemsg Views module.
 */

/**
 * A function class to represent a join and create the SQL necessary
 * to implement the join.
 *
 *
 * join definition
 *   - table: table to join (right table)
 *   - field: field to join on (right field)
 *   - left_table: The table we join to
 *   - left_field: The field we join to
 *   - type: either LEFT (default) or INNER
 *   - extra: Either a string that's directly added, or an array of items, 
 *     where each item represents additional JOIN criteria:
 *     array(
 *       $right_field_name => array(
 *         'field' => $left_field_name,
 *         'table' => $left_table,
 *         'operator' => $operator,
 *       )
 *     )
 *     In this example, variables are:
 *       - $right_field_name
 *           name of the field in the joined (right) table 
 *       - $left_field_name
 *           name of the field in the left side of join criteria
 *       - $left_table
 *           name of the table in the left side of join criteria
 *       - $operator
 *           operator between left and right side in the join criteria
 *     
 *   - extra type: How all the extras will be combined. Either AND or OR. Defaults to AND.
 */
class views_multiple_field_join extends views_join {
  /**
   * Build the SQL for the join this object represents.
   */
  function join($table, &$query) {
    if (empty($this->definition['table formula'])) {
      $right_table = "{" . $this->table . "}";
    }
    else {
      $right_table = $this->definition['table formula'];
    }

    if ($this->left_table) {
      $left = $query->get_table_info($this->left_table);
      $left_field = "$left[alias].$this->left_field";
    }
    else {
      // This can be used if left_field is a formula or something. It should be used only *very* rarely.
      $left_field = $this->left_field;
    }

    $output = " $this->type JOIN $right_table $table[alias] ON $left_field = $table[alias].$this->field";
    // Tack on the extra.
    if (isset($this->extra)) {
      if (is_array($this->extra)) {
        $extras = array();
        foreach ($this->extra as $right_field => $info) {
          $left = $query->get_table_info($info['table']);
          $field = $info['field'];
          $left_field = "$left[alias].$field";
          $right_field = "$table[alias].$right_field";
          $operator = $info['operator'];
          $extras[] = "$left_field $operator $right_field";
        }

        if ($extras) {
          if (count($extras) == 1) {
            $output .= ' AND ' . array_shift($extras);
          }
          else {
            $output .= ' AND (' . implode(' ' . $this->extra_type . ' ', $extras) . ')';
          }
        }
      }
      else if ($this->extra && is_string($this->extra)) {
        $output .= " AND ($this->extra)";
      }
    }
    return $output;
  }
}
