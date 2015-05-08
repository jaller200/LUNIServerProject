##Guidelines
Stay consistant with conventions. Look at how the original code is written, and stick with it unless there is good reason not to.

All contributers, even those with write access, should create a pull request if they would like to have their code added to the main repository. This way it is easier for everyones changes to be made without loosing work from other commits.

Don't change the directory structure unless you need to. This makes changes extremely hard to track, as Git treats this as deleting then readding every file. Only  proficient contributers should do this, and then only for good reason and in a separate commit from any changes in the files, so they those can easily be seen.

Version adjustment and switching between stable and dev folders should only be done by  proficient contributers, and should always be done when a release is warrented and agreed upon (a release build should also be generated and distributed at this point).

If as someone with write access you're unsure about merging a commit, add a comment to the pull request and discuss with the other devs.
