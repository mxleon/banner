<?php

/**
 * @file
 * Serves administration pages of banner.
 */


/**
 * Form builder; The banner default settings form.
 *
 * @see banner_default_form()
 */
function banner_default_form() {
  $variables = variable_get('banner_image_settings');
  $languages = language_list();
  foreach ($languages as $lancode => $language) {
    if (!$language->enabled) continue;
    $form['banner_' . $lancode] = array(
      '#type' => 'fieldset',
      //'#description' => banner_image_get_descriptions($variables),
      '#title' => $lancode .' | '. $language->native,
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );
    $form['banner_' . $lancode]['banner'] = array(
      '#type' => 'fieldset',
      '#title' => t('Banner default settings'),
    );
    $form['banner_' . $lancode]['banner']['banner_file_'. $lancode .'_default'] = array(
      '#title' => t('Upload banner image'),
      '#type' => 'managed_file',
      '#description' => banner_image_get_descriptions($variables),
      '#default_value' => variable_get('banner_file_'. $lancode .'_default', ''),
      '#upload_validators' => banner_image_upload_validators($variables),
      '#upload_location' => 'public://' . $variables['file_directory'],
      //'#required' => TRUE,
    );
  }
  //$form['#validate'] = array('banner_default_file_set_usage');
  $form['#submit'] = array('banner_default_file_set_status');

  return system_settings_form($form);
}

/**
 * Form builder; The banner conent type settings form.
 *
 * @see banner_content_type_form()
 */
function banner_content_type_form() {
  $node_types = node_type_get_types();
  if (is_array($node_types) && count($node_types) > 0) {
    $variables = variable_get('banner_image_settings');
    $languages = language_list();
    foreach ($languages as $lancode => $language) {
      if (!$language->enabled) continue;
      $form['banner_' . $lancode] = array(
        '#type' => 'fieldset',
        //'#description' => banner_image_get_descriptions($variables),
        '#title' => $lancode .' | '. $language->native,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      foreach ($node_types as $node_type) {
        $form['banner_' . $lancode][$node_type->type] = array(
          '#type' => 'fieldset',
          '#title' => t('@type', array('@type' => $node_type->name)),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );
        $form['banner_' . $lancode][$node_type->type]['banner_file_'. $lancode .'_type_'. $node_type->type] = array(
          '#title' => t('Upload banner image'),
          '#type' => 'managed_file',
          '#description' => banner_image_get_descriptions($variables),
          '#default_value' => variable_get('banner_file_'. $lancode .'_type_'. $node_type->type, ''),
          '#upload_validators' => banner_image_upload_validators($variables),
          '#upload_location' => 'public://' . $variables['file_directory'],
        );
      }
    }
    //$form['#validate'] = array('banner_content_type_file_set_usage');
    $form['#submit'] = array('banner_content_type_file_set_status');

    return system_settings_form($form);
  }
}

/**
 * Form builder; The banner taxonomy settings form.
 *
 * @see banner_taxonomy_form()
 */
function banner_taxonomy_form() {
  $taxonomy_vocabularies = taxonomy_get_vocabularies();
  if (is_array($taxonomy_vocabularies) && count($taxonomy_vocabularies) > 0) {
    $variables = variable_get('banner_image_settings');
    $languages = language_list();
    foreach ($languages as $lancode => $language) {
      if (!$language->enabled) continue;
      $form['banner_' . $lancode] = array(
        '#type' => 'fieldset',
        '#title' => $lancode .' | '. $language->native,
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      foreach ($taxonomy_vocabularies as $taxonomy) {
        $form['banner_' . $lancode][$taxonomy->machine_name] = array(
          '#type' => 'fieldset',
          '#title' => t('@type', array('@type' => $taxonomy->name)),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );
        $form['banner_' . $lancode][$taxonomy->machine_name]['banner_file_'. $lancode .'_taxo_'. $taxonomy->machine_name] = array(
          '#title' => t('Upload banner image'),
          '#type' => 'managed_file',
          '#description' => banner_image_get_descriptions($variables),
          '#default_value' => variable_get('banner_file_'. $lancode .'_taxo_'. $taxonomy->machine_name, ''),
          '#upload_validators' => banner_image_upload_validators($variables),
          '#upload_location' => 'public://' . $variables['file_directory'],
        );
      }
    }
    //$form['#validate'] = array('banner_taxonomy_file_set_usage');
    $form['#submit'] = array('banner_taxonomy_file_set_status');

    return system_settings_form($form);
  }
}

/**
 * Form builder; The banner custom settings form.
 *
 * @see banner_custom_form_callback()
 */
function banner_custom_form_callback() {
  return drupal_get_form('banner_custom_form');
}

/**
 * Form definition: orders rules.
 */
function banner_custom_form($form, &$form_state) {
  $rules = db_query('SELECT * FROM {banner_rules} ORDER BY rid');
  $delta = $rules->rowCount();
  
  // Default message if no rules.
  if ($delta == 0) {
    $form['rules'] = array(
      '#markup' => '<p>' . t('No rules have been created yet.') . '</p>',
    );
  }
  else {
    $form['rules'] = array(
      '#tree' => TRUE,
      '#theme' => 'custom_banner_rules_order',
    );
    foreach ($rules as $rule) {
      $form['rules'][$rule->rid] = array(
        'admin_title' => array(
          '#markup' => '<strong>' . check_plain($rule -> admin_title) . '</strong>',
        ),
        'banner_path' => array(
          '#markup' => check_plain($rule -> banner_path),
        ),
        'operations' => array(
          'edit-link' => array(
            '#type' => 'link',
            '#title' => t('edit'),
            '#href' => 'admin/config/banner/custom/edit/' . $rule->rid,
          ),
          'delete-link' => array(
            '#type' => 'link',
            '#title' => t('delete'),
            '#href' => 'admin/config/banner/custom/delete/' . $rule->rid,
          ),
        ),
      );
    }
  }

  return $form;
}

/**
 * Handles form submission for banner_default_form() for set file status to permanent.
 */
function banner_default_file_set_status($form, &$form_state) {
  $languages = language_list();
  foreach ($languages as $lancode => $language) {
    if (!$language->enabled) continue;
    banner_file_set_status($form_state['values']['banner_file_'. $lancode .'_default'], 'banner_file_'. $lancode .'_default');
  }
}

/**
 * Handles form submission for banner_content_type_form() for set file status to permanent.
 */
function banner_content_type_file_set_status($form, &$form_state) {
  $node_types = node_type_get_types();
  if (is_array($node_types) && count($node_types) > 0) {
    $languages = language_list();
    foreach ($languages as $lancode => $language) {
      if (!$language->enabled) continue;
      foreach ($node_types as $node_type) {
        banner_file_set_status($form_state['values']['banner_file_'. $lancode .'_type_'. $node_type->type], 'banner_file_'. $lancode .'_type_'. $node_type->type);
      }
    }
  }
}

/**
 * Handles form submission for banner_content_type_form() for set file status to permanent.
 */
function banner_taxonomy_file_set_status($form, &$form_state) {
  $taxonomy_vocabularies = taxonomy_get_vocabularies();
  if (is_array($taxonomy_vocabularies) && count($taxonomy_vocabularies) > 0) {
    $languages = language_list();
    foreach ($languages as $lancode => $language) {
      if (!$language->enabled) continue;
      foreach ($taxonomy_vocabularies as $taxonomy) {
        banner_file_set_status($form_state['values']['banner_file_'. $lancode .'_taxo_'. $taxonomy->machine_name], 'banner_file_'. $lancode .'_taxo_'. $taxonomy->machine_name);
      }
    }
  }
}

/**
 * Handles form submission for banner_default_form() for set file status to permanent.
 */
function banner_file_set_status($fid, $name) {
  if ($fid != 0) {
    $file = file_load($fid);
    $file->status = FILE_STATUS_PERMANENT;
    file_save($file);
    file_usage_add($file, 'banner', 'file', $fid);
    variable_set($name, $file->fid);
  } elseif ($fid == 0) {
    $fid = variable_get($name, FALSE);
    $file = $fid ? file_load($fid) : FALSE;
    if ($file) {
      file_usage_delete($file, 'banner', 'file', $fid);
      file_delete($file);
    }
    variable_set($name, FALSE);
  }

	//db_query('UPDATE {file_managed} SET status = :file_status WHERE fid = :fid', array(':file_status' => $value, ':fid' => $fid));
}

/**
 * Form builder; The banner image styles setting form.
 *
 * @see banner_image_admin_settings_form()
 */
function banner_image_admin_settings_form() {
  $variables = variable_get('banner_image_settings');

  $image_styles = image_styles();
  $styles = array();
  foreach ($image_styles as $image_style) {
    $styles[$image_style['name']] = $image_style['name'];
  }

  $form['image_styles']['style_name'] = array(
    '#type' => 'select',
    '#title' => t('Image style'),
    '#options' => $styles,
    '#default_value' => $variables['style_name'],
    '#empty_option'   => t('None (original image)'),
    '#description' => t('Control all banner for !url.', array("!url" => l(t('Image styles'), "admin/config/media/image-styles"))),
    '#weight' => 1,
  );
  $form['image_styles']['file_directory'] = array(
    '#type' => 'textfield',
    '#title' => t('File directory'),
    '#default_value' => $variables['file_directory'],
    '#description' => t('Optional subdirectory within the upload destination where files will be stored. Do not include preceding or trailing slashes.'),
    '#element_validate' => array('_banner_image_settings_file_directory_validate'),
    '#weight' => 2,
  );

  // Make the extension list a little more human-friendly by comma-separation.
  $extensions = str_replace(' ', ', ', $variables['file_extensions']);
  $form['image_styles']['file_extensions'] = array(
    '#type' => 'textfield',
    '#title' => t('Allowed file extensions'),
    '#default_value' => $extensions,
    '#description' => t('Separate extensions with a space or comma and do not include the leading dot.'),
    '#element_validate' => array('_banner_image_settings_extensions'),
    '#weight' => 3,
    // By making this field required, we prevent a potential security issue
    // that would allow files of any type to be uploaded.
    '#required' => TRUE,
  );

  // Add maximum and minimum resolution settings.
  $max_resolution = explode('x', $variables['max_resolution']) + array('', '');
  $form['image_styles']['max_resolution'] = array(
    '#type' => 'item',
    '#title' => t('Maximum image resolution'),
    '#element_validate' => array('_banner_image_settings_resolution_validate'),
    '#weight' => 4.1,
    '#field_prefix' => '<div class="container-inline">',
    '#field_suffix' => '</div>',
    '#description' => t('The maximum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Leave blank for no restriction. If a larger image is uploaded, it will be resized to reflect the given width and height. Resizing images on upload will cause the loss of <a href="http://en.wikipedia.org/wiki/Exchangeable_image_file_format">EXIF data</a> in the image.'),
  );
  $form['image_styles']['max_resolution']['max_x'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum width'),
    '#title_display' => 'invisible',
    '#default_value' => $max_resolution[0],
    '#size' => 5,
    '#maxlength' => 5,
    '#field_suffix' => ' x ',
  );
  $form['image_styles']['max_resolution']['max_y'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum height'),
    '#title_display' => 'invisible',
    '#default_value' => $max_resolution[1],
    '#size' => 5,
    '#maxlength' => 5,
    '#field_suffix' => ' ' . t('pixels'),
  );

  $min_resolution = explode('x', $variables['min_resolution']) + array('', '');
  $form['image_styles']['min_resolution'] = array(
    '#type' => 'item',
    '#title' => t('Minimum image resolution'),
    '#element_validate' => array('_banner_image_settings_resolution_validate'),
    '#weight' => 4.2,
    '#field_prefix' => '<div class="container-inline">',
    '#field_suffix' => '</div>',
    '#description' => t('The minimum allowed image size expressed as WIDTHxHEIGHT (e.g. 640x480). Leave blank for no restriction. If a smaller image is uploaded, it will be rejected.'),
  );
  $form['image_styles']['min_resolution']['min_x'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum width'),
    '#title_display' => 'invisible',
    '#default_value' => $min_resolution[0],
    '#size' => 5,
    '#maxlength' => 5,
    '#field_suffix' => ' x ',
  );
  $form['image_styles']['min_resolution']['min_y'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum height'),
    '#title_display' => 'invisible',
    '#default_value' => $min_resolution[1],
    '#size' => 5,
    '#maxlength' => 5,
    '#field_suffix' => ' ' . t('pixels'),
  );
  $form['image_styles']['max_filesize'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum upload size'),
    '#default_value' => $variables['max_filesize'],
    '#description' => t('Enter a value like "512" (bytes), "80 KB" (kilobytes) or "50 MB" (megabytes) in order to restrict the allowed file size. If left empty the file sizes will be limited only by PHP\'s maximum post and file upload sizes (current limit <strong>%limit</strong>).', array('%limit' => format_size(file_upload_max_size()))),
    '#size' => 10,
    '#element_validate' => array('_banner_image_settings_max_filesize'),
    '#weight' => 5,
  );

  $form ['#submit'] = array('banner_image_settings_submit');

  return system_settings_form($form);
}

/**
 * Returns HTML for the custom banner form.
 */
function theme_custom_banner_rules_order($variables) {
  $element = $variables['element'];
  drupal_add_tabledrag('custom-banner', 'order', 'sibling', 'rule-weight');

  $variables = array(
    'header' => array(
      t('Administrative Title'),
      t('Banner file'),
      array(
        'data' => t('Operations'),
        'colspan' => '2',
      ),
    ),
    'rows' => array(),
    'attributes' => array('id' => 'custom-banner'),
  );

  // Generate table of draggable menu names.
  foreach (element_children($element) as $rule) {
    // Render the title, enabled, and weight columns.
    $file_object	= db_query('SELECT * FROM {file_managed} WHERE fid = :fid', array(':fid' => $element[$rule]['banner_path']['#markup']))->fetchAssoc();
    $data = array(
      drupal_render($element[$rule]['admin_title']),
      l($file_object['filename'], str_replace('public:/', variable_get('file_public_path'), $file_object['uri']), array('attributes' => array('target' => '_blank'))),
    );
    // Render the operations links.
    foreach (element_children($element[$rule]['operations']) as $op) {
      $data[] = array(
        'data' => drupal_render($element[$rule]['operations'][$op]),
        'class' => array('menu-operations'),
      );
    }
    $variables['rows'][] = array(
      'data' => $data,
      'class' => array('draggable'),
    );
  }

  return theme('table', $variables);
}

/**
 * Menu callback; Adds rules.
 */
function banner_custom_add_form($form, &$form_state) {
  return custom_banner_form($form, $form_state);
}

/**
 * Returns form to add or edit a menu position rule.
 */
function custom_banner_form($form, &$form_state) {
  // Set the default values.
  $rid          = !empty($form_state['#custom-banner']['rid'])         ? $form_state['#custom-banner']['rid'] : '';
  $admin_title  = !empty($form_state['#custom-banner']['admin_title']) ? $form_state['#custom-banner']['admin_title'] : '';
  $banner_path  = !empty($form_state['#custom-banner']['banner_path']) ? $form_state['#custom-banner']['banner_path'] : '';
  $conditions   = !empty($form_state['#custom-banner']['conditions'])  ? $form_state['#custom-banner']['conditions'] : '';
  
  // Add default values for conditions when has destination arguments.
  if ($conditions == '' && isset($_GET['destination']) && trim($_GET['destination']) != '') $conditions = trim($_GET['destination']);

  $variables = variable_get('banner_image_settings');
  
  $form['rid'] = array(
    '#type' => 'hidden',
    '#value' => $rid,
  );
  $form['admin_title'] = array(
    '#type' => 'textfield',
    '#default_value' => $admin_title,
    '#title' => t('Administrative title'),
    '#description' => t('This title will be used administratively to identify this custom banner.'),
    '#required' => TRUE,
  );
  $form['banner_path'] = array(
    '#title' => t('Upload banner image'),
    '#type' => 'managed_file',
    '#description' => banner_image_get_descriptions($variables),
    '#default_value' => $banner_path,
    '#upload_validators' => banner_image_upload_validators($variables),
    '#upload_location' => 'public://' . $variables['file_directory'],
    '#required' => TRUE,
  );
  $form['conditions'] = array(
    '#type' => 'textarea',
    '#title' => t('Conditions settings'),
    '#default_value' => $conditions,
    '#description' => t('The condition path settings of custom banner.'),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  // Form validation and submission.
  $form['#validate'][] = 'banner_custom_file_set_usage';
  $form['#submit'][] = 'custom_banner_form_submit';
  $form['#submit'][] = 'banner_custom_file_set_status';
  
  return $form;
}

/**
 * Handles form submission for custom_banner_form().
 */
function custom_banner_form_submit($form, &$form_state) {
  $rule = array(
    'admin_title' => $form_state['values']['admin_title'],
    'banner_path' => $form_state['values']['banner_path'],
    'conditions' => $form_state['values']['conditions'],
  );
  
  // Add the rule to the database.
  if ($form_state['values']['rid'] == '') {
    custom_banner_add_rule($rule);
    drupal_set_message(t('Custom banner has been added.'));
  }
  // Update an exisiting rule.
  else {
    $rule['rid'] = $form_state['values']['rid'];
    custom_banner_edit_rule($rule);
    drupal_set_message(t('Custom banner has been modified.'));
  }

  $form_state['redirect'] = 'admin/config/banner/custom';
}

/**
 * Adds a banner rule.
 */
function custom_banner_add_rule($rule) {
  $fields = array(
    'admin_title' => $rule['admin_title'],
    'banner_path' => $rule['banner_path'],
    'conditions'  => serialize($rule['conditions']),
  );
  $rid = db_insert('banner_rules')
    ->fields($fields)
    ->execute();
}

/**
 * Edits a banner rule.
 */
function custom_banner_edit_rule($rule) {
  $fields = array(
    'admin_title' => $rule['admin_title'],
    'banner_path' => $rule['banner_path'],
    'conditions'  => serialize($rule['conditions']),
  );
  // Update the rule.
  db_update('banner_rules')
    ->condition('rid', $rule['rid'])
    ->fields($fields)
    ->execute();
}

/**
 * Menu callback; Edits banner rules.
 */
function banner_custom_edit_form($form, &$form_state, $rid = 0) {
  // Make sure rid is set.
  if ($rid == 0) {
    drupal_goto('admin/config/banner/custom');
    return;
  }
  // Grab the rule from the database.
 $form_state['#custom-banner'] = custom_banner_read_rule($rid);
  return custom_banner_form($form, $form_state);
}

/**
 * Menu callback: confirms deletion of banner rule.
 */
function banner_custom_delete_confirm($form, &$form_state, $rid = 0) {
  // Make sure rid is set.
  if ($rid == 0) {
    drupal_goto('admin/config/banner/custom');
    return;
  }
  $form['rid'] = array('#type' => 'hidden', '#value' => $rid);

  $custom_banner = custom_banner_read_rule($rid);
  $title = $custom_banner['admin_title'];

  return confirm_form($form, t('Are you sure you want to delete the %title banner rule?', array('%title' => $title)), 'admin/config/banner/custom/edit/' . $rid, NULL, t('Delete'), t('Cancel'));
}

/**
 * Handles form submission for banner_custom_delete_form().
 */
function banner_custom_delete_confirm_submit($form, &$form_state) {
  $custom_banner = custom_banner_read_rule($form_state['values']['rid']);
  $title = $custom_banner['admin_title'];

  custom_banner_delete($form_state['values']['rid']);

  drupal_set_message(t('The %title banner rule has been deleted.', array('%title' => $title)));
  $form_state['redirect'] = 'admin/config/banner/custom';
}

/**
 * Deletes a menu position rule.
 */
function custom_banner_delete($rid) {
  db_delete('banner_rules')
    ->condition('rid', $rid)
    ->execute();
}

/**
 * Retrieves a banner rule from the database.
 *
 * @param $rid
 *   The ID of the requested rule.
 * @return
 *   An associative array for the requested rule.
 */
function custom_banner_read_rule($rid) {
  $rule = db_query('SELECT * FROM {banner_rules} WHERE rid = :rid', array(':rid' => $rid))->fetchAssoc();
  $rule['conditions'] = unserialize($rule['conditions']);
  return $rule;
}

/**
 * The the image attributes of the banner.
 *
 * @see banner_image_get_descriptions()
 */
function banner_image_get_descriptions($variables) {
  $descriptions = array();

  // Cap the upload size according to the PHP limit.
  $max_filesize = parse_size(file_upload_max_size());
  if (!empty($variables['max_filesize']) && parse_size($variables['max_filesize']) < $max_filesize) {
    $max_filesize = parse_size($variables['max_filesize']);
  }
  $descriptions[] = t('Files must be less than !size.', array('!size' => '<strong>' . format_size($max_filesize) . '</strong>'));
  if (!empty($variables['file_extensions'])) {
    $descriptions[] = t('Allowed file types: !extensions.', array('!extensions' => '<strong>' . check_plain($variables['file_extensions']) . '</strong>'));
  }
  if (!empty($variables['max_resolution']) && !empty($variables['min_resolution'])) {
    $max = $variables['max_resolution'];
    $min = $variables['min_resolution'];
    if ($min && $max && $min == $max) {
      $descriptions[] = t('Images must be exactly !size pixels.', array('!size' => '<strong>' . $max . '</strong>'));
    }
    elseif ($min && $max) {
      $descriptions[] = t('Images must be between !min and !max pixels.', array('!min' => '<strong>' . $min . '</strong>', '!max' => '<strong>' . $max . '</strong>'));
    }
    elseif ($min) {
      $descriptions[] = t('Images must be larger than !min pixels.', array('!min' => '<strong>' . $min . '</strong>'));
    }
    elseif ($max) {
      $descriptions[] = t('Images must be smaller than !max pixels.', array('!max' => '<strong>' . $max . '</strong>'));
    }
  }

  return implode('<br />', $descriptions);
}

/**
 * Retrieves the upload validators for a file field.
 *
 * @param $variables
 *   A array.
 *
 * @return
 *   An array suitable for passing to file_save_upload() or the file field
 *   element's '#upload_validators' property.
 */
function banner_image_upload_validators($variables) {

  // Cap the upload size according to the PHP limit.
  $max_filesize = parse_size(file_upload_max_size());
  if (!empty($variables['max_filesize']) && parse_size($variables['max_filesize']) < $max_filesize) {
    $max_filesize = parse_size($variables['max_filesize']);
  }

  $validators = array();

  // There is always a file size limit due to the PHP server limit.
  $validators['file_validate_size'] = array($max_filesize);

  // Add the extension check if necessary.
  if (!empty($variables['file_extensions'])) {
    $validators['file_validate_extensions'] = array($variables['file_extensions']);
  } //else {
    $validators['file_validate_is_image'] = array();
  //}
  
  if (!empty($variables['max_resolution']) && !empty($variables['min_resolution'])) {
    $validators['file_validate_image_resolution'] = array($variables['max_resolution'], $variables['min_resolution']);
  }

  return $validators;
}
