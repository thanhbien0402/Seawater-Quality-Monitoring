import { getTelemetryTb, loginTb, readSetting, sendDataTb, updateTask } from "@/actions"
import { createClient } from "@supabase/supabase-js";



const supabaseUrl = process.env.NEXT_PUBLIC_SUPABASE_URL!;
const supabaseKey = process.env.SUPABASE_SERVICE_ROLE_KEY!;
const supabase = createClient(supabaseUrl, supabaseKey);

const getStatusSwitch = async () => {
    try {
        const setting: any = await supabase
            .from('settings')
            .select('*')
            .order('created_at', { ascending: false })
            .limit(1)
        const { token, error: loginError } = await loginTb();

        if (setting && setting.data && setting.data.length > 0) {
            let { entityId } = setting.data[0]

            const data = await getTelemetryTb(entityId, token as string, ['pumpStatus'])

            if (data?.pumpStatus.length > 0) {
                return data.pumpStatus[0].value === '0' ? false : true
            }
        }
    } catch (error) {
        console.log(error)
    }
}


export default async function handler(req: any, res: any) {

    if (req.method !== 'GET') {
        return res.status(405).json({ message: 'Method Not Allowed' })
    }

    try {
        // Get the current date and time
        const currentDate = new Date()
        const currentDateStr = currentDate.toISOString().split('T')[0]; // yyyy-mm-dd
        const currentTimeStr = currentDate.toTimeString().split(' ')[0].substring(0, 5) // HH:MM

        console.log('currentDate: ' + currentDateStr)
        console.log('\ncurrentTime: ' + currentTimeStr)

        // Fetch tasks that need to be triggered from Supabase
        const { data: tasks, error } = await supabase
            .from('tasks')
            .select('*')
            .eq('task_date', currentDateStr)
            .eq('task_time', currentTimeStr)

        console.log('tasks >>>>', tasks)

        if (error) {
            return res.status(500).json({ message: 'Error fetching tasks', error })
        }

        const { token, error: loginError } = await loginTb();
        if (loginError) {
            return res.status(500).json({ message: 'Error logging into ThingsBoard', error: loginError });
        }

        // Iterate through tasks and trigger them
        for (const task of tasks) {
            const { id, task_type, trigger_type, created_by } = task;

            const currentPumpStatus = await getStatusSwitch()
            const desiredPumpStatus = trigger_type === 'true' ? true : false

            if (currentPumpStatus === desiredPumpStatus) { break }

            // Call Thingboard API to trigger the device status
            await sendDataTb(task_type, trigger_type === 'true' ? true : false, token as string)

            // Mark the task as triggered
            await supabase
                .from('tasks')
                .update({ status: 'success' })
                .eq('id', id)
                .eq('created_by', created_by);
        }

        return res.status(200).json({ message: 'Tasks triggered successfully' })
    } catch (error) {
        return res.status(500).json({ message: 'Server Error', error })
    }
}
