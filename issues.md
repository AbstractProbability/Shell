1.  log.c: log.log_execute() depends on execute.executor()
    execute.c: execute.execute_one()/execute.execute_all() depends on multiple log functionality
    this is all resolved by the linker so there is no circular dependency, but there exists an architectural circular dependency.

