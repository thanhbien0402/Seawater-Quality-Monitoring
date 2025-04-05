import cron from 'node-cron'
import axios from 'axios';

// Schedule the cron job to run every minute
cron.schedule('* * * * *', async () => {
    try {
        console.log('Running cron job to invoke API...');
        const response = await axios.get('http://localhost:3000/api/triggerTask');
        console.log('API Response:', response.data);
    } catch (error) {
        console.error('Error running cron job:', error);
    }
});

// This is necessary to keep the script running
console.log('Cron job has been scheduled to run every minute.');