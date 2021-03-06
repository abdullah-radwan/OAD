# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## 1.0.5 - 2019-03-05
### Added
- Quazip with threading.
- Allow multiple add-ons to override one add-on.
- Detect if the add-on is already installed.
- Show a message if uninstallation failed.
- Failed message if adding add-on to the database failed.
- Show a dialog during operations.
- Show all add-ons in the database option.
- Remove the installed file if the installation failed.

### Fixed
- Files in the main directory are backed up as folders.

## 1.0.4a - 2018-12-08
### Added
- New add-ons to the database.

### Fixed
- Parent folder isn't detected correctly in zip file.

## 1.0.4 - 2018-11-19
### Added
- Move to trash option in settings.
- A warning is shown if the installation failed.
- New add-ons to the database.

### Fixed (Special thanks to Zan (Zandy12))
- Snapshot removes all Orbiter installation files.
- Parent folder not being detected correctly.
- Backup files are saved in the main folder, not in the backup folder.
- Add-on file is removed when the installation is failed.
- Add-ons appears repeatedly in installed addons.
- Program crashes doing long install and import process.
- Overrider add-ons aren't detected correctly.

## 1.0.3 - 2018-10-20
### Added
- Support to Zip files.
- Ignored files/folders can be specified per add-on.
- More than 50 add-ons to the database.

### Fixed
- Backup files are saved in the main program folder.

### Changed
- Remove button in database editor removes the selected file instead of the add-on.
- New entries in add ignored file/folder are added to old entries, not override.
- Remap the project structure.

## 1.0.2 - 2018-10-05
### Added
- Ability to manage multiple Orbiter installation.
- Ability to detect base add-ons.
- Settings window.
- Snapshots.
- Database editor.
- 4 add-ons to the default database.
- Ignored files/folders.

### Fixed
- Several language mistakes.
- Faulty config writer.

### Changed
- Install/Add dialog renamed to install dialog, and can be used only to install.
 To add an add-on, add it in the database editor.


## 1.0.1 - 2018-09-21
### Added
- Enable and disable feature.
- OverriderMap section in config file.
- GitHub repository releases URL in about dialog.
- Rescan add-ons button.

### Fixed
- Empty directory leaved when uninstall an add-on.
- Readme file and program language mistakes.
- Faulty orbiter.exe detector.
- Include config.cfg file.

### Changed
- Config file AddonsData section renamed to DatabaseMap.
- Rename the add-on name in the database with the entered name if it contains the same files.
- Re-format the manual.

### Removed
- Remove GPL3 License from about dialog.
- Remove (User) suffix when add a new add-on.