## Version 2.2.0-alpha (19.06.2020)
- Added access to the cached guilds
- Added access to the cached users
- Implemented GUILD_PRESENCES event. You can now retieve informations when a user updates his online state or activity state.
- You can now set the activity state via `SetActivity`
- You can now set the online state via `SetState`
- You can now set the afk state vie `SetAFK`
- Added `QuitAsync` to avoid deadlocks.
- Added guild icons
- Bugfix: If the bot resumes the guild cache is cleared and never restored.

## Version 2.1.1-alpha (06.06.2020)
- Bugfix for Issue #7 the bot crashes randomly.

## Version 2.1.0-alpha (01.06.2020)
- Added direct message support

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