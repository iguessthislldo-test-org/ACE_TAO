/* -*- C++ -*- */
//=============================================================================
/**
 *  @file    FT_ReplicationManagerFaultAnalyzer.cpp
 *
 *  $Id$
 *
 *  This file is part of TAO's implementation of Fault Tolerant CORBA.
 *
 *  @author Steve Totten <totten_s@ociweb.com>
 */
//=============================================================================

#include "orbsvcs/FT_ReplicationManager/FT_ReplicationManagerFaultAnalyzer.h"
#include "orbsvcs/CosNotifyCommC.h"
#include "orbsvcs/FT_NotifierC.h"
#include "orbsvcs/FT_ReplicationManager/FT_ReplicationManager.h"
#include "orbsvcs/FT_ReplicationManager/FT_FaultEventDescriptor.h"
#include "orbsvcs/PortableGroup/PG_Property_Utils.h"
#include "orbsvcs/PortableGroup/PG_Operators.h"
#include "orbsvcs/FaultTolerance/FT_IOGR_Property.h"
#include <tao/debug.h>
#include <iostream>

#define INTEGRATED_WITH_REPLICATION_MANAGER 1

ACE_RCSID (FT_ReplicationManagerFaultAnalyzer,
           FT_ReplicationManagerFaultAnalyzer,
           "$Id$")

/// Constructor.
TAO::FT_ReplicationManagerFaultAnalyzer::FT_ReplicationManagerFaultAnalyzer (
  const TAO::FT_ReplicationManager * replication_manager)
  : replication_manager_ (
      ACE_const_cast (TAO::FT_ReplicationManager *, replication_manager))
{
}

/// Destructor.
TAO::FT_ReplicationManagerFaultAnalyzer::~FT_ReplicationManagerFaultAnalyzer ()
{
}

// Validate the event to make sure it is one we can handle.
// If it is not an event we can handle, this function logs the error
// and returns -1.
int TAO::FT_ReplicationManagerFaultAnalyzer::validate_event_type (
  const CosNotification::StructuredEvent & event)
{
  // Delegate to base class.
  //@@ Visual C++ 6.0 won't compile this if I include the namespace name
  // on the base class.
  // return TAO::FT_DefaultFaultAnalyzer::validate_event_type (event);
  return FT_DefaultFaultAnalyzer::validate_event_type (event);
}

/// Analyze a fault event.
int TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event (
  const CosNotification::StructuredEvent & event)
{
  int result = 0;

  const CosNotification::FilterableEventBody & filterable =
    event.filterable_data;
  CORBA::ULong item_count = filterable.length ();
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
  if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
  {
    for (CORBA::ULong n_prop = 0; n_prop < item_count; ++n_prop)
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT(
          "TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event: "
          "Property Name: <%s>\n"),
        filterable[n_prop].name.in()
      ));
    }
  }

  // Populate a TAO::FT_FaultEventDescriptor structure from the
  // properties in the event.
  TAO::FT_FaultEventDescriptor fault_event_desc;

  // Extract the location.
  if (result == 0)
  {
    result = this->get_location (
      filterable[1].value, fault_event_desc.location.out());
  }

  // CORBA 3.0.2, section 23.4.5.1 states:
  //
  //   The fault detector may or may not set the TypeId and
  //   ObjectGroupId fields with the following interpretations:
  //   - Neither is set if all objects at the given location have failed.
  //   - TypeId is set and ObjectGroupId is not set if all objects at
  //     the given location with the given type have failed.
  //   - Both are set if the member with the given ObjectGroupId at the
  //     given location has failed.

  if ((result == 0) && (item_count == 2))
  {
    // All objects at location failed.
    fault_event_desc.all_at_location_failed = 1;
  }

  if ((result == 0) && (item_count == 3))
  {
    // All objects of type at location failed.
    fault_event_desc.all_of_type_at_location_failed = 1;
    result = this->get_type_id (
      filterable[2].value, fault_event_desc.type_id.out());
  }

  if ((result == 0) && (item_count == 4))
  {
    // An object (replica) at a location failed.
    fault_event_desc.object_at_location_failed = 1;
    result = this->get_type_id (
      filterable[2].value, fault_event_desc.type_id.out());
    if (result == 0)
    {
      result = this->get_object_group_id (
        filterable[3].value, fault_event_desc.object_group_id);
    }
  }

#if (INTEGRATED_WITH_REPLICATION_MANAGER == 1)
  // A specific object at a location failed.
  if ((result == 0) && (fault_event_desc.object_at_location_failed == 1))
  {
    result = this->single_replica_failure (fault_event_desc);
  }

  // All objects at location failed.
  if ((result == 0) && (fault_event_desc.all_at_location_failed == 1))
  {
    result = this->location_failure (fault_event_desc);
  }

  // All objects of type at location failed.
  if ((result == 0) && (fault_event_desc.all_of_type_at_location_failed == 1))
  {
    result = this->type_failure (fault_event_desc);
  }
#endif /* (INTEGRATED_WITH_REPLICATION_MANAGER == 1) */

  // Debugging support.
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
  if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
  {
    fault_event_desc.dump ();
  }

  return result;
}

// Extract a string type_id from CORBA::Any.
// Caller owns the string returned via <type_id>.
int TAO::FT_ReplicationManagerFaultAnalyzer::get_type_id (
  const CORBA::Any& val, FT::TypeId_out type_id)
{
  const char* type_id_value;
  if ((val >>= type_id_value) == 0)
  {
    ACE_ERROR_RETURN ((LM_ERROR,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::get_type_id: "
        "Could not extract TypeId value from any.\n")),
      -1);
  }

  std::cout << std::endl << std::endl << std::endl
            << type_id_value
            << std::endl << std::endl << std::endl;

  // Make a deep copy of the TypeId string.
  type_id = CORBA::string_dup (type_id_value);
  return 0;
}

// Extract the ObjectGroupId from CORBA::Any.
int TAO::FT_ReplicationManagerFaultAnalyzer::get_object_group_id (
  const CORBA::Any& val, FT::ObjectGroupId& id)
{
  FT::ObjectGroupId temp_id = (FT::ObjectGroupId)0;
  if ((val >>= temp_id) == 0)
  {
    ACE_ERROR_RETURN ((LM_ERROR,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::get_object_group_id: "
        "Could not extract ObjectGroupId value from any.\n")),
      -1);
  }
  id = temp_id;
  return 0;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::get_location (
  const CORBA::Any& val, FT::Location_out location)
{
  const FT::Location* temp_loc;
  if ((val >>= temp_loc) == 0)
  {
    ACE_ERROR_RETURN ((LM_ERROR,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::get_location: "
        "Could not extract Location value from fault event.\n")),
      -1);
  }
  // Make a deep copy of the Location.
  ACE_NEW_RETURN (location, FT::Location (*temp_loc), -1);
  return 0;
}

//
//TODO: Use TAO_PG::find() to get property values from properties
// instead of all these specific "get" functions.
//

// Get the MembershipStyle property.
int TAO::FT_ReplicationManagerFaultAnalyzer::get_membership_style (
  const FT::Properties & properties,
  FT::MembershipStyleValue & membership_style)
{
  FT::Name prop_name (1);
  prop_name.length (1);
  prop_name[0].id = CORBA::string_dup (FT::FT_MEMBERSHIP_STYLE);
  int result = 0;

  FT::Value value;
  if (TAO_PG::get_property_value (prop_name, properties, value)
    && ((value >>= membership_style) == 1))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event: "
          "MembershipStyle is <%d>:\n"),
        membership_style
      ));
    }
  }
  else
  {
    result = -1;
  }

  return result;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::get_replication_style (
  const FT::Properties & properties,
  FT::ReplicationStyleValue & replication_style)
{
  FT::Name prop_name (1);
  prop_name.length (1);
  prop_name[0].id = CORBA::string_dup (FT::FT_REPLICATION_STYLE);
  int result = 0;

  FT::Value value;
  if (TAO_PG::get_property_value (prop_name, properties, value)
    && ((value >>= replication_style) == 1))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event: "
          "ReplicationStyle is <%d>:\n"),
        replication_style
      ));
    }
  }
  else
  {
    result = -1;
  }

  return result;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::get_minimum_number_members (
  const FT::Properties & properties,
  FT::MinimumNumberMembersValue & minimum_number_members)
{
  FT::Name prop_name (1);
  prop_name.length (1);
  prop_name[0].id = CORBA::string_dup (FT::FT_MINIMUM_NUMBER_MEMBERS);
  int result = 0;

  FT::Value value;
  if (TAO_PG::get_property_value (prop_name, properties, value)
    && ((value >>= minimum_number_members) == 1))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event: "
          "MinimumNumberMembers is <%d>:\n"),
        minimum_number_members
      ));
    }
  }
  else
  {
    result = -1;
  }

  return result;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::get_initial_number_members (
  const FT::Properties & properties,
  FT::InitialNumberMembersValue & initial_number_members)
{
  FT::Name prop_name (1);
  prop_name.length (1);
  prop_name[0].id = CORBA::string_dup (FT::FT_INITIAL_NUMBER_MEMBERS);
  int result = 0;

  FT::Value value;
  if (TAO_PG::get_property_value (prop_name, properties, value)
    && ((value >>= initial_number_members) == 1))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::analyze_fault_event: "
          "InitialNumberMembers is <%d>:\n"),
        initial_number_members
      ));
    }
  }
  else
  {
    result = -1;
  }

  return result;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::get_factories (
  const FT::Properties & properties,
  FT::FactoryInfos_out factories)
{
  FT::Name prop_name (1);
  prop_name.length (1);
  prop_name[0].id = CORBA::string_dup (FT::FT_FACTORIES);
  int result = 0;

  FT::FactoryInfos_var temp_factories;
  FT::Value value;
  if (TAO_PG::get_property_value (prop_name, properties, value) == 1)
  {
    if ((value >>= temp_factories) == 0)
    {
      ACE_ERROR ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::get_factories: "
          "Could not extract Factories from properties.\n")
      ));
      result = -1;
    }
    else
    {
      // Make a deep copy of the Factories.
      ACE_NEW_RETURN (factories, FT::FactoryInfos (temp_factories.in()), -1);
      result = 0;
    }
  }
  else
  {
    ACE_ERROR ((LM_ERROR,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::get_factories: "
        "Could not find Factories property.\n")
    ));
    result = -1;
  }
  return result;
}

int TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member (
  FT::ObjectGroup_ptr iogr,
  const FT::Location & location,
  int & object_is_primary)
{

  //@@ Q: How do we determine if this was a primary that faulted?
  //@@ A: Get the TagFTGroupTaggedComponent from the IOGR and search
  // for the primary, using the TAO_FT_IOGR_Property helper class.
  // Then, compare the TypeId and Location of the failed object with
  // those of the primary.  If they match, it was a primary fault.

  int result = 0;
  object_is_primary = 0;

  ACE_TRY_NEW_ENV
  {
    // Create an "empty" TAO_FT_IOGR_Property and use it to get the
    // tagged component.
    TAO_FT_IOGR_Property temp_ft_prop;
    FT::TagFTGroupTaggedComponent ft_group_tagged_component;
    CORBA::Boolean got_tagged_component =
      temp_ft_prop.get_tagged_component (
          iogr, ft_group_tagged_component ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;
    if (got_tagged_component)
    {
      // Create a new TAO_FT_IOGR_Property with the tagged
      // component.
      TAO_FT_IOGR_Property ft_prop (ft_group_tagged_component);

      // Check to see if a primary is set.
      CORBA::Boolean primary_is_set = ft_prop.is_primary_set (
        iogr ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;
      if (primary_is_set)
      {
        // Get the primary object.
        CORBA::Object_var primary_obj = ft_prop.get_primary (
          iogr ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;
        if (CORBA::is_nil (primary_obj.in()))
        {
          ACE_ERROR_RETURN ((LM_ERROR,
            ACE_TEXT (
              "TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member: "
              "Could not get primary IOR from IOGR.\n")),
            -1);
        }

        // Get the object reference of the failed member.
        CORBA::Object_var failed_obj =
          this->replication_manager_->get_member_ref (
            iogr, location ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;
        if (CORBA::is_nil (failed_obj.in()))
        {
          ACE_ERROR_RETURN ((LM_ERROR,
            ACE_TEXT (
              "TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member: "
              "Could not get IOR of failed member from IOGR.\n")),
            -1);
        }

        // Are the two object refs (primary and failed) equivalent?
        CORBA::Boolean equiv = primary_obj->_is_equivalent (
          failed_obj.in() ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;
        if (equiv)
        {
          object_is_primary = 1;
          result = 0;
        }
      }
      else  // primary is not set
      {
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member: "
            "Primary is not set on IOGR.\n")
        ));
        result = -1;
      }
    }
    else // could not get tagged component
    {
      ACE_ERROR ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member: "
          "Could not get tagged component from IOGR.\n")
      ));
      result = -1;
    }
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::is_primary_member: ")
    );
    result = -1;
  }
  ACE_ENDTRY;

  return result;
}

// Handle a single replica failure.
int TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure (
  TAO::FT_FaultEventDescriptor & fault_event_desc)
{
  int result = 0;
  FT::ObjectGroup_var the_object_group = FT::ObjectGroup::_nil();
  FT::Properties_var properties;

  ACE_TRY_NEW_ENV
  {
    // Get the object group reference based on the ObjectGroupId.
    the_object_group =
      this->replication_manager_->get_object_group_ref_from_id (
        fault_event_desc.object_group_id
        ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // This should not happen, but let us be safe.
    if (CORBA::is_nil (the_object_group.in()))
    {
      ACE_ERROR ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not get ObjectGroup reference from ObjectGroupId: <%Q>.\n"),
          fault_event_desc.object_group_id
      ));
      ACE_THROW (PortableGroup::ObjectGroupNotFound ());
    }

    // Get the properties associated with this ObjectGroup.
    properties = this->replication_manager_->get_properties (
      the_object_group.in()
      ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      ACE_TEXT (
        "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: ")
    );
    result = -1;
  }
  ACE_ENDTRY;
  ACE_CHECK_RETURN (-1);

  if (result == 0)
  {
    // Get the MembershipStyle property.
    FT::MembershipStyleValue membership_style;
    result = this->get_membership_style (properties.in(), membership_style);
    if (result != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not extract MembershipStyle from properties on "
          "ObjectGroup with id <%Q>.\n"),
        fault_event_desc.object_group_id),
        -1);
    }
    else
    {
      fault_event_desc.membership_style = membership_style;
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
      if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
            "MembershipStyleValue = <%d>"),
            fault_event_desc.membership_style
        ));
      }
    }

    // Get the ReplicationStyle property.
    FT::ReplicationStyleValue replication_style;
    result = this->get_replication_style (properties.in(), replication_style);
    if (result != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not extract ReplicationStyle from properties on "
          "ObjectGroup with id <%Q>.\n"),
        fault_event_desc.object_group_id),
        -1);
    }
    else
    {
      fault_event_desc.replication_style = replication_style;
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
      if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
            "ReplicationStyleValue = <%d>"),
            fault_event_desc.replication_style
        ));
      }
    }

    // Get the MinimumNumberMembers property.
    FT::MinimumNumberMembersValue minimum_number_members;
    result = this->get_minimum_number_members (
      properties.in(), minimum_number_members);
    if (result != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not extract MinimumNumberMembers from properties on "
          "ObjectGroup with id <%Q>.\n"),
        fault_event_desc.object_group_id),
        -1);
    }
    else
    {
      fault_event_desc.minimum_number_members = minimum_number_members;
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
      if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
            "MinimumNumberMembers = <%d>"),
            fault_event_desc.minimum_number_members
        ));
      }
    }

    // Get the InitialNumberMembers property.
    FT::InitialNumberMembersValue initial_number_members;
    result = this->get_initial_number_members (
      properties.in(), initial_number_members);
    if (result != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not extract InitialNumberMembers from properties on "
          "ObjectGroup with id <%Q>.\n"),
        fault_event_desc.object_group_id),
        -1);
    }
    else
    {
      fault_event_desc.initial_number_members = initial_number_members;
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
      if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
            "InitialNumberMembers = <%d>"),
            fault_event_desc.initial_number_members
        ));
      }
    }

    // Get the Factories property.
    result = this->get_factories (
      properties.in(),
      fault_event_desc.factories.out());
    if (result != 0)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Could not extract Factories from properties on "
          "ObjectGroup with id <%Q>.\n"),
        fault_event_desc.object_group_id),
        -1);
    }
    else
    {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
      if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
      {
        ACE_DEBUG ((LM_DEBUG,
          ACE_TEXT (
            "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
            "Got Factories from properties on "
            "ObjectGroup with id <%Q>.\n"),
            fault_event_desc.object_group_id
        ));
      }
    }

  }

  // If the ReplicationStyle is COLD_PASSIVE, WARM_PASSIVE, or
  // SEMI_ACTIVE, we can see if it was the primary replica that
  // failed.
  if ((result == 0) &&
      (fault_event_desc.replication_style == FT::COLD_PASSIVE ||
       fault_event_desc.replication_style == FT::WARM_PASSIVE ||
       fault_event_desc.replication_style == FT::SEMI_ACTIVE))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Checking to see if failed replica was the primary for "
          "ObjectGroup with id <%Q>.\n"),
          fault_event_desc.object_group_id
      ));
    }
    result = this->is_primary_member (
      the_object_group.in(),
      fault_event_desc.location.in(),
      fault_event_desc.object_is_primary);
  }

  // If the MembershipStyle is FT::MEMB_INF_CTRL (infrastructure
  // controlled) and the primary has faulted, establish a new primary.
  // We get back a new object group.
  FT::ObjectGroup_var new_object_group;
  if ((result == 0) &&
      (fault_event_desc.membership_style == FT::MEMB_INF_CTRL) &&
      (fault_event_desc.object_is_primary == 1))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Setting new primary for "
          "ObjectGroup with id <%Q>.\n"),
          fault_event_desc.object_group_id
      ));
    }
    result = this->set_new_primary (
      the_object_group.in(),
      fault_event_desc,
      new_object_group.out());
    the_object_group = new_object_group;
  }

  // If the MembershipStyle is FT::MEMB_INF_CTRL (infrastructure
  // controlled) and the number of remaining members is less than
  // the MinimumNumberMembers property, add new members.
  // We get back a new object group.
  if ((result == 0) &&
      (fault_event_desc.membership_style == FT::MEMB_INF_CTRL))
  {
#if (TAO_DEBUG_LEVEL_NEEDED == 1)
    if (TAO_debug_level > 6)
#endif /* (TAO_DEBUG_LEVEL_NEEDED == 1) */
    {
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::single_replica_failure: "
          "Potentially adding new members to "
          "ObjectGroup with id <%Q>.\n"),
          fault_event_desc.object_group_id
      ));
    }
    result = this->add_members (
      the_object_group.in(),
      fault_event_desc,
      new_object_group.out());
    the_object_group = new_object_group;
  }

  return result;
}

// Choose a new primary member for the ObjectGroup.
// Sets <new_iogr> and returns 0 on success.
// Returns -1 on failure.
int TAO::FT_ReplicationManagerFaultAnalyzer::set_new_primary (
  FT::ObjectGroup_ptr iogr,
  TAO::FT_FaultEventDescriptor & fault_event_desc,
  FT::ObjectGroup_out new_iogr)
{
  int result = 0;
  new_iogr = FT::ObjectGroup::_nil ();

  ACE_TRY_NEW_ENV
  {
    // Remove the old primary member from the object group.
    FT::ObjectGroup_var temp_iogr =
      this->replication_manager_->remove_member (
        iogr,
        fault_event_desc.location.in()
        ACE_ENV_ARG_DECL);
    ACE_TRY_CHECK;

    // Get the locations of the remaining members of the object group.
    FT::Locations_var locations =
      this->replication_manager_->locations_of_members (
        temp_iogr.in()
        ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Choose the first location as our new primary location.
    if (locations->length() >= 1)
    {
      new_iogr = this->replication_manager_->set_primary_member (
        temp_iogr.in(),
        (*locations)[0]
        ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;
    }
    else
    {
      ACE_ERROR_RETURN ((LM_ERROR,
        ACE_TEXT (
          "TAO::FT_ReplicationManagerFaultAnalyzer::set_new_primary: "
          "No locations remaining in ObjectGroup with id <%Q>.\n"),
          fault_event_desc.object_group_id),
        -1);
    }
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      "TAO::FT_ReplicationManagerFaultAnalyzer::set_new_primary: ");
    result = -1;
  }
  ACE_ENDTRY;

  return result;
}

// While the number of members in the object group is less than
// the MinimumNumberMembers property, add new members.
// Sets <new_iogr> and returns 0 on success.
// Returns -1 on failure.
int TAO::FT_ReplicationManagerFaultAnalyzer::add_members (
  FT::ObjectGroup_ptr iogr,
  TAO::FT_FaultEventDescriptor & fault_event_desc,
  FT::ObjectGroup_out new_iogr)
{
  int result = 0;
  new_iogr = FT::ObjectGroup::_nil ();

  ACE_TRY_NEW_ENV
  {
    // Get current number of members in object group
    // (same as number of locations).
    FT::Locations_var locations =
      this->replication_manager_->locations_of_members (
        iogr
        ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;
    CORBA::ULong num_members = locations->length();

    // If it is less than the MinimumNumberMembers property, add
    // new members.
    if (num_members < fault_event_desc.minimum_number_members)
    {
      //@@ To create a member, we need to know the ObjectGroup,
      //   Location, TypeId, and Criteria.

      // Get the factory registry from the Replication Manager.
      PortableGroup::Criteria fake_criteria;
      PortableGroup::FactoryRegistry_var factory_registry =
        this->replication_manager_->get_factory_registry (
          fake_criteria ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;


      // @@ DLW SAYS: we need to find out the role played by this object
      // group so we can use the correct set of factories.
      // Get the list of factories for the type of the failed replica.
      CORBA::String_var type_id;
      PortableGroup::FactoryInfos_var factories_by_type =
          factory_registry->list_factories_by_role (
          fault_event_desc.type_id.in(), type_id ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      //
      // Build a set of locations of factories for this type that we
      // can use to create new members (i.e., at locations where
      // members do not currently exist).
      //
      FT_Location_Set valid_locations;

      // For each factory that can be used for this type...
      for (CORBA::ULong f=0; f<factories_by_type->length(); ++f)
      {
        // ...insert its location into valid_locations set.
        valid_locations.insert (factories_by_type[f].the_location);
      }

      // Now remove any locations where members already exist.
      for (CORBA::ULong m=0; m<num_members; ++m)
      {
        if (valid_locations.find (locations[m]))
          valid_locations.remove (locations[m]);
      }

      // The valid_locations set now contains all the factory
      // locations we can use to add members to this object group.
      // So, now we add new members until we reach
      // the value of the MinimumNumberMembers property.
      FT::Location_var good_location;
      for (FT_Location_Set::iterator iter (valid_locations);
           iter.next (good_location.out()) &&
           fault_event_desc.minimum_number_members > num_members;
           iter.advance(), ++num_members)
      {
        // Create a new member of the object group at this location.
        new_iogr = this->replication_manager_->create_member (
          iogr,
          good_location.in(),
          fault_event_desc.type_id.in(),
          fake_criteria
          ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;

        // Stop adding members when we reach the value of the
        // MinimumNumberMembers property.
        // if (num_members++ >= fault_event_desc.minimum_number_members)
            // break;
      }

    }
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      "TAO::FT_ReplicationManagerFaultAnalyzer::add_members: ");
    result = -1;
  }
  ACE_ENDTRY;

  return result;
}

// Handle a location failure.
int TAO::FT_ReplicationManagerFaultAnalyzer::location_failure (
  TAO::FT_FaultEventDescriptor & fault_event_desc)
{
  int result = 0;

  // To handle a location failure, we should:
  // - Unregister all the factories at that location.
  //   (We do this first so that we don't try to create a new replica
  //   at that location for any of the affected object groups.)
  // - Determine all the object groups that had members at that
  //   location.
  // - Handle each one of them as a single replica failure. 

  ACE_TRY_NEW_ENV
  {
    // Get the factory registry from the Replication Manager.
    PortableGroup::Criteria fake_criteria;
    PortableGroup::FactoryRegistry_var factory_registry =
      this->replication_manager_->get_factory_registry (
        fake_criteria ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Unregister all factories at the failed location.
    factory_registry->unregister_factory_by_location (
      fault_event_desc.location.in() ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Determine all the object groups that had members at that
    // location.
    PortableGroup::ObjectGroups_var object_groups_at_location =
      this->replication_manager_->groups_at_location (
      fault_event_desc.location.in() ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Handle each one of them as a single replica failure.
    for (CORBA::ULong i=0;
         result==0 && i<object_groups_at_location->length();
         ++i)
    {
      // Get the object group id.
      fault_event_desc.object_group_id =
        this->replication_manager_->get_object_group_id (
          object_groups_at_location[i] ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Get type id of this object group.
      fault_event_desc.type_id =
        this->replication_manager_->type_id (
          object_groups_at_location[i] ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Handle it as a single replica failure.
      result = this->single_replica_failure (fault_event_desc);
    }
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      "TAO::FT_ReplicationManagerFaultAnalyzer::location_failure: ");
    result = -1;
  }
  ACE_ENDTRY;

  return result;
}

// Handle a type failure.
int TAO::FT_ReplicationManagerFaultAnalyzer::type_failure (
  TAO::FT_FaultEventDescriptor & fault_event_desc)
{
  int result = 0;

  // To handle a type failure, we should:
  // - Unregister the factory at the location of the failure
  //   that is associated with the failed type.
  //   (We do this first so that we don't try to create a new replica
  //   with that factory for any of the affected object groups.)
  // - Determine all the object groups that had members at that
  //   location of that type.
  // - Handle each one of them as a single replica failure. 

  ACE_TRY_NEW_ENV
  {
    // Get the factory registry from the Replication Manager.
    PortableGroup::Criteria fake_criteria;
    PortableGroup::FactoryRegistry_var factory_registry =
      this->replication_manager_->get_factory_registry (
        fake_criteria ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Unregister the factory at the failed location associated with
    // the role.
    //@@ Using type_id as the role for now.
    factory_registry->unregister_factory (
      fault_event_desc.type_id.in(),
      fault_event_desc.location.in()
      ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // Get all the object groups that had members at that
    // location.
    PortableGroup::ObjectGroups_var object_groups_at_location =
      this->replication_manager_->groups_at_location (
      fault_event_desc.location.in() ACE_ENV_ARG_PARAMETER);
    ACE_TRY_CHECK;

    // For each one, if it was of the same type as the failed type,
    // handle it as a single replica failure.
    for (CORBA::ULong i=0;
         result==0 && i<object_groups_at_location->length();
         ++i)
    {
      // Get the object group id.
      fault_event_desc.object_group_id =
        this->replication_manager_->get_object_group_id (
          object_groups_at_location[i] ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Get type id of this object group.
      FT::TypeId_var type_id =
        this->replication_manager_->type_id (
          object_groups_at_location[i] ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // If the type id is the same as the failed type id...
      if (ACE_OS::strcmp (type_id.in(), fault_event_desc.type_id.in()) == 0)
      {
        // Handle it as a single replica failure.
        fault_event_desc.type_id = CORBA::string_dup (type_id.in());
        result = this->single_replica_failure (fault_event_desc);
      }
    }
  }
  ACE_CATCHANY
  {
    ACE_PRINT_EXCEPTION (
      ACE_ANY_EXCEPTION,
      "TAO::FT_ReplicationManagerFaultAnalyzer::type_failure: ");
    result = -1;
  }
  ACE_ENDTRY;

  return result;
}

// Template instantiations.
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Unbounded_Set<FT::Location>;
template class ACE_Unbounded_Set_Iterator<FT::Location>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Unbounded_Set<FT::Location>
#pragma instantiate ACE_Unbounded_Set_Iterator<FT::Location>

#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */

