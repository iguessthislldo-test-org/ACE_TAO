// -*- C++ -*-
// $Id$

//=============================================================================
/**
 * @file  Planner.h
 *
 * This file contains the Planner abstract base class definition for planners,
 * which direct planning and mediate communication between other planning
 * objects.
 *
 * @author  John S. Kinnebrew <john.s.kinnebrew@vanderbilt.edu>
 */
//=============================================================================

#ifndef SA_POP_PLANNER_H_
#define SA_POP_PLANNER_H_


#include <string>
#include <set>
#include <map>
#include "SA_POP_Types.h"
#include "PlanStrategy.h"
#include "SchedStrategy.h"
#include "WorkingPlan.h"
#include "TaskMap.h"
#include "OutAdapter.h"
#include "PlanCommands.h"
#include "SANet/SANet.h"

namespace SA_POP {

  /**
   * @class Planner
   *
   * @brief Abstract base class for planners, which direct planning and
   *        mediate communication between other planning objects.
   */
  class Planner {
  public:

    /// Constructor.
    Planner (void);

    /// Destructor.
    virtual ~Planner (void);


    // ************************************************************************
    // Initialization methods.
    // ************************************************************************

    /// Add planning objects.
    /**
     * @param sanet  Spreading activation network.
     *
     * @param plan_strat  PlanStrategy object for planning.
     *
     * @param sched_strat  SchedStrategy object for scheduling.
     *
     * @param working_plan  WorkingPlan object for holding plan in progress.
     *
     * @param task_map  TaskMap object for associating tasks with
     *                  implementations and resources.
     */
    virtual void set_objects (SANet::Network *sanet, PlanStrategy *plan_strat,
      SchedStrategy *sched_strat, WorkingPlan *working_plan,
      TaskMap *task_map);

    /// Add output adapter.
    /**
     * @param out  OutAdapter to add.
     */
    virtual void add_out_adapter (OutAdapter *out);


    virtual void generate_all_threats(void);



    // ************************************************************************
    // Planning/re-planning methods.
    // ************************************************************************

    /// Run planning.
    /**
     * @param sa_max_steps  Maximum steps to run spreading activation.
     *
     * @param goal  Goal for which to plan.
     *
     * @return  True if planning succeeded, false otherwise.
     */
    virtual bool plan (size_t sa_max_steps, SA_POP::Goal goal);
    
    /// Replan with new goal.
    /**
     * @param sa_max_steps  Maximum steps to run spreading activation.
     *
     * @param goal  Goal for which to plan.
     *
     * @return  True if planning succeeded, false otherwise.
     */
    virtual bool replan (size_t sa_max_steps, SA_POP::Goal goal);

    /// Replan with current goal.
    /**
     * @param sa_max_steps  Maximum steps to run spreading activation.
     *
     * @return  True if planning succeeded, false otherwise.
     */
    virtual bool replan (size_t sa_max_steps);



    // ************************************************************************
    // Plan accessor methods.
    // ************************************************************************

    /// Get current plan.
    /**
     * @return  Reference to current plan.
     */
    virtual const Plan& get_plan (void);

    /// Calculate expected utility (EU) of a plan for current conditions.
    /**
     * @param plan  Plan for which to calculate EU.
     *
     * @return  Expected utility of provided plan given current conditions.
     */
    virtual EUCalc calc_plan_eu (Plan plan);

    /// Get last set of expected utility changes.
    /**
     * @return  Reference to last set of expected utility changes.
     */
    virtual const TaskEUMap& get_eu_changes (void);



    // ************************************************************************
    // Task network display/output methods.
    // ************************************************************************

    /// Print a text representation of the task network.
    /**
     * @param strm  Output stream on which to print network representation.
     *
     * @param verbose  Whether to print verbose representation.
     */
    virtual void print_sanet (std::basic_ostream<char, std::char_traits<char> >& strm
      = std::cout, bool verbose = false);

    /// Print the graphviz representation of the task network.
    /**
     * @param strm  Output stream on which to print the graphviz representation.
     *
     * @param graphmap  Mapping from ??? to ???.
     */
    virtual void print_sanet_graphviz (std::basic_ostream<char, std::char_traits<char> >& strm,
      std::map<std::string, std::string>& graphmap);



    // ************************************************************************
    // Recursive planning/scheduling methods.
    // ************************************************************************

    /// Recursively plan (satisfy all open conditions & schedule constraints).
    /**
     * @return  True if fully satisfied plan found, false otherwise.
     */
    virtual bool recurse_plan (void);

    /// Recursively schedule (satisfy schedule constraints and continue
    /// recursive planning).
    /**
     * @param task_inst  Current task instance being tried in the plan.
     *
     * @return  True if fully satisfied plan found, false otherwise.
     */
    virtual bool recurse_sched (TaskInstID task_inst);

    /// Satisfy scheduling constraints in fully instantiated plan (no
    /// recursive call backs).
    /**
     * @return  True if fully satisfied plan found, false otherwise.
     */
    virtual bool full_sched ();



    // ************************************************************************
    // Planning/scheduling command execution and undo methods.
    // ************************************************************************

    /// Execute a command (adding it as the current command).
    /**
     * @param command  The command to add and execute.
     */
    virtual void execute_command (PlanCommand *command);

    /// Undo and remove command.
    /**
     * @param id  The ID of the command to undo and remove.
     *
     * @exception InvalidCommand  The ID provided does not correspond to top
     *                            command.
     */
    virtual void undo_command (CommandID id);

    /// Add a command to be executed later with execute_next().
    /**
     * @param command  The command to add for execution.
     */
    virtual void add_command (PlanCommand *command);

    /// On current command, undo last execution (if any) & execute next option.
    /**
     * @param id  The ID of the command to undo and remove.
     *
     * @exception InvalidCommand  The ID provided does not correspond to top
     *                            command.
     *
     * @return True if command had an option to execute, false otherwise.
     */
    virtual bool try_next (CommandID id);

    /// Undo and remove all commands back to specified point.
    /**
     * @param id  The ID of the command to undo and remove through.
     */
    virtual void undo_through (CommandID id);

    /// Get the current command ID.
    /**
     * @return  The ID of the current command
     */
    virtual CommandID cur_command_id();


    // ************************************************************************
    // Condition update methods (environment/system state or goal changes).
    // ************************************************************************

    /// Update a condition's current value (probability of being true).
    /**
     * @param cond_id  ID of the condition.
     *
     * @param true_prob  New probability that condition is true.
     */
    virtual void update_cond_val (CondID cond_id, double true_prob);



    // ************************************************************************
    // General task/condition accessor methods.
    // ************************************************************************

    /// Get a condition's current value (probability of being true).
    /**
     * @param cond_id  ID of the condition.
     *
     * @return  Probability that condition is true.
     */
    virtual double get_cond_val (CondID cond_id);

    /// Get all goals.
    /**
     * @return  Set of condition ids and associated utilities.
     */
    virtual const GoalMap& get_goals (void);

    /// Get a task's name.
    /**
     * @param task_id  ID of the task.
     *
     * @return  Task name.
     */
    virtual std::string get_task_name (TaskID task_id);

    /// Get a condition's name.
    /**
     * @param cond_id  ID of the condition.
     *
     * @return  Condition name.
     */
    virtual std::string get_cond_name (CondID cond_id);

    /// Get a condition's type/kind.
    /**
     * @param cond_id  ID of the condition.
     *
     * @return  Condition type.
     */
    virtual CondKind get_cond_type (CondID cond_id);

    /// Get a task's future expected utility (EU) from spreading activation.
    /// (NOTE: Future EU is based on whatever spreading
    /// activation has already been executed).
    /**
     * @param task_id  ID of the task.
     *
     * @return  Future task expected utility.
     */
    virtual Utility get_task_sa_eu (TaskID task_id);

    /// Get all preconditions of a task.
    /**
     * @param task_id  ID of the task.
     *
     * @return  Set of all preconditions with associated values.
     */
    virtual CondSet get_preconds (TaskID task_id);

    /// Get currently unsatisfied preconditions of a task.
    /**
     * @param task_id  ID of the task.
     *
     * @return  Set of all unsatisfied preconditions with associated values.
     */
    virtual CondSet get_unsat_preconds (TaskID task_id);

    /// Get all effects of a task.
    /**
     * @param task_id  ID of the task.
     *
     * @return  Set of all effects with associated values.
     */
    virtual CondSet get_effects (TaskID task_id);

    /// Get the probability of a task's effect.
    /**
     * @param task_ID  ID of task.
     *
     * @param cond_ID  ID of effect condition.
     *
     * @returns  Probability of effect given successful task execution.
     */
    virtual Probability get_effect_prob (SANet::TaskID id, SANet::CondID cond_ID);

    /// Get all tasks that satisfy a condition.
    /**
     * @param cond_id  ID of the condition.
     *
     * @return  Set of all tasks that satisfy the given condition.
     */
    virtual TaskSet get_satisfying_tasks (Condition cond);

    /// Get ports for a causal link.
    /**
     * @param task1_id  ID of start task in causal link.
     *
     * @param cond_id  ID of the condition node in causal link.
     *
     * @param task2_id  ID of end task in causal link.
     */
    virtual LinkPorts get_clink_ports (TaskID task1_id, CondID cond_id,
      TaskID task2_id);



    // ************************************************************************
    // Planning task/condition info accessor methods.
    // ************************************************************************

    /// Get task ID of a task instance.
    /**
     * @param inst_id  The task instance ID.
     *
     * @return  ID of the task of this task instance.
     */
    virtual TaskID get_task_from_inst (TaskInstID inst_id);

    /// Get all current causal link threats.
    /**
     * @return  Set of all current causal link threats.
     */
    virtual CLThreatSet get_all_threats (void);



    // ************************************************************************
    // TaskMap accessor methods (resources, task->implementations,
    // and implementation->resources).
    // ************************************************************************

    /// Get all implementations of a task.
    /**
     * @param task_id  ID of the task.
     *
     * @return  The set of all implementations (ids) for the given task.
     */
    virtual TaskImplSet get_all_impls (TaskID task_id);

    /// Get task implementation.
    /**
     * @param impl_id  The task implementation ID.
     *
     * @return  Reference to the task implementation.
     */
    virtual TaskImpl *get_impl (TaskImplID impl_id);

    /// Get utilization info of a task implementation for a resource.
    /**
     * @param impl_id  The task implementation ID.
     *
     * @param resource_id  The resource ID.
     *
     * @return  The quantity of resource used.
     */
    virtual ResourceValue get_resource_usage (TaskImplID impl_id,
      ResourceID resource_id);

    /// Get all resources used by a task implementation.
    /**
     * @param impl_id  The task implementation ID.
     *
     * @return  The set of all resources used (with associated usage values).
     */
    virtual ResourceMap get_all_resources (TaskImplID impl_id);




    // ************************************************************************
    // Scheduling Precedence Graph/Time Windows/Resources accessor methods.
    // ************************************************************************

    /// Get the Task instances in a particular set of the specified task instance
    /**
     * @param task_inst The Task Instance whose Precedence Set has been queried
     *
     * @param prec_rel The Precedence relation to the task_inst
     *
     * @return A pointer to the Task Instance Set that has the relation prec_rel to task_inst
     */
    virtual const TaskInstSet* get_prec_insts (TaskInstID task_inst, PrecedenceRelation prec_rel);

    /// Get the Start Window of the Task instance
    /**
     * @param task_inst The Task Instance whose Start Window is required
     *
     * @return The Start Window of task_inst
     */
    virtual TimeWindow get_start_window (TaskInstID task_inst);

    /// Get the End Window of the Task instance
    /**
     * @param task_inst The Task Instance whose End Window is required
     *
     * @return The End Window of task_inst
     */
    virtual TimeWindow get_end_window (TaskInstID task_inst);

    ///Get the duration of a task instance
    /**
     * @param task_inst The task instance of which the duration is returned
     *
     * @return The duration of the task instance
     */
    virtual TimeValue get_duration(TaskInstID task_inst);

    /// Get task implementation ID of a task instance.
    /**
     * @param inst_id  The task instance ID.
     *
     * @return  The task implementation ID of this task instance.
     */
    virtual TaskImplID get_task_impl_from_inst (TaskInstID inst_id);

    /// Get the capacity of a resource.
    /**
     * @param res_id The resource ID whose capacity that we want to get.
     *
     * @return The capacity of the resource
     */
    virtual ResourceValue get_capacity (ResourceID res_id);

    /// Get the Causal and Scheduling orderings to this task instance
    /**
     * @param inst_id The task instance to which all orderings are required
     */
    virtual TaskInstSet before_orderings (TaskInstID inst_id);

    /// Get the Causal and Scheduling orderings from this task instance
    /**
     * @param inst_id The task instance from which all orderings are required
     */
    virtual TaskInstSet after_orderings (TaskInstID inst_id);

    /// Get all the task instances
    virtual TaskInstSet get_all_insts();

    /// Check if the instance ID already exists and is being reused.
    /**
     * @param task_inst The task instance being checked
     *
     * @return  True If this task instance already exists.
     */
    virtual bool inst_exists (TaskInstID task_inst);

    /// Get task implementation for a task instance.
    /**
     * @param task_inst  The task instance.
     *
     * @return The task implementation ID.
     */
    virtual TaskImplID get_impl_id (TaskInstID task_inst);


    /// (re) Set Effect Link
    /**
     * @param task_inst  ID of the task.
     * @param task_inst  ID of the condition.
     * @param task_inst  The LinkWeight
     * 
     *
     * @return nothing
     */
    virtual void update_effect (SANet::TaskID tsk, SANet::CondID cnd, SANet::LinkWeight weight);

    virtual WorkingPlan*  get_working_plan(void){return this->working_plan_;};

    virtual void set_backtrack_cmd_id(CommandID cmd){backtrack_cmd = cmd;};

  protected:
    /// Threshold for current probability of a condition to be satisfied.
    const Probability cond_prob_thresh_;

    /// Flag for whether planning objects have been set.
    bool has_objs_;

    /// Spreading activation network.
    SANet::Network *sanet_;

    /// PlanStrategy object for planning.
    PlanStrategy *plan_strat_;

    /// SchedStrategy object for scheduling.
    SchedStrategy *sched_strat_;

    /// WorkingPlan object for holding plan in progress.
    WorkingPlan *working_plan_;

    /// TaskMap object for associating tasks with implementations and resources.
    TaskMap *task_map_;

    /// OutAdapter objects.
    std::set <OutAdapter *> out_adapters_;

    /// Current complete plan.
    Plan plan_;

    /// Last set of expected utility changes.
    TaskEUMap eu_changes_;

    /// Current command.
    PlanCommand *cur_cmd_;

    /// Notify all output adapters that plans have changed.
    virtual void notify_plan_changed (void);

	CommandID backtrack_cmd;

	CommandID not_backtracking;
  };

};  /* SA_POP namespace */

#endif /* SA_POP_PLANNER_H_ */
