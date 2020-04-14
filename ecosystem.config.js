module.exports = {
  apps : [{
    name: 'intercom-notificatinos',
    script: 'app.js',
    instances: 1,
    autorestart: true,
    watch: false,
    max_memory_restart: '1G',
    env: {
      NODE_ENV: 'development'
    },
    env_production: {
      NODE_ENV: 'production'
    }
  }],

  deploy : {
    production : {
      user : 'webapps',
      host : '10.0.0.201',
      ref  : 'origin/master',
      repo : 'git@github.com:carbonaro/intercom-notifications.git',
      path : '/home/webapps/intercom-notifications',
      'post-deploy' : 'cp /home/webapps/intercom-notifications/shared/config/default.json /home/webapps/intercom-notifications/current/config/ && npm install && pm2 reload ecosystem.config.js --env production'
    }
  }
};
