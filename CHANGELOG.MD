## Version 2.0.0-alpha (31.05.2020)
- Bugfix: Multiple command parameters now working correctly
- Added role management for commands
- Added builtin command for manage roles of commands
- The help command is now a command instead of a hard coded if statement
- Inside the Discord Developer Portal you need `Server Members Intent` to tick on, because this is needed for the role management.
- Every command registration need now to specify a default right for a command.
- The `IController` interfaces constructor must now called from the derived controller.
- Little changes of the documentation.

## Version 1.0.0-prealpha (22.05.2020)
- Initial release
- Added Voice connection
- Added Music queue API
- Added commands system
- Added message handling