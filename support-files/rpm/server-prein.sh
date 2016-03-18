# Check if we can safely upgrade.  An upgrade is only safe if it's from one
# of our RPMs in the same version family.

installed=`rpm -q --whatprovides mysql-server 2> /dev/null`
if [ $? -eq 0 -a -n "$installed" -a -z "$FORCE_UPGRADE" ]; then
  installed=`echo "$installed"|sed -n 1p`
  if ! expr "$installed" : '.*[Mm]aria' >/dev/null; then
    cat <<HERE >&2

******************************************************************
A $installed server package is installed. It might be not completely compatible
with MariaDB. Some files may be installed to different locations, including log
files and the service startup script in %{_sysconfdir}/init.d/.
You can force the upgrade by setting the FORCE_UPGRADE=1 environment variable,
but a manual upgrade is recommended:

- Ensure that you have a complete, working backup of your data and my.cnf
  files
- Shut down the MySQL server cleanly
- Remove the existing MySQL packages.  Usually this command will
  list the packages you should remove:
  rpm -qa | grep -i '^mysql-'

  You may choose to use 'rpm --nodeps -ev <package-name>' to remove
  the package which contains the mysqlclient shared library.  The
  library will be reinstalled by the MariaDB-shared package.
- Install the new MariaDB packages
- Ensure that the MariaDB server is started
- Run the 'mysql_upgrade' program

This is a brief description of the upgrade process.  Important details
can be found in the MariaDB manual, in the Upgrading section.
******************************************************************
HERE
    exit 1
  fi
fi
