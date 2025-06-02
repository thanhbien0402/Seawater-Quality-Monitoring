'use server'

import axios from '@/config/axiosTb'
import { AxiosResponse } from 'axios'
import createSupabaseServerClient from '@/lib/supabase/server'
import { redirect } from 'next/navigation'
import { revalidatePath, unstable_noStore as noStore } from 'next/cache'

type ResponseData = {
  token: string
  refreshToken: string
}

type LoginResult = {
  token?: string
  refreshToken?: string
  error?: Error
}

type TaskData = {
  task_type: string
  task_date: string
  task_time: string
  trigger_type: boolean
}

// ! Add-ons function
function dateAndTimeToCronWithSeconds(date: Date | undefined, time: string): string {
  // parse "HH:MM"
  const [hourStr, minuteStr] = time.split(':')
  const hours = parseInt(hourStr, 10)
  const minutes = parseInt(minuteStr, 10)
  if (
    isNaN(hours) || hours < 0 || hours > 23 ||
    isNaN(minutes) || minutes < 0 || minutes > 59
  ) {
    throw new Error(`Invalid time format (“${time}”), expected "HH:MM"`)
  }

  if (!date) return `${minutes} ${hours} * * *`

  const dayOfMonth = date?.getDate()          // 1 - 31
  const month = date?.getMonth() + 1     // 1 - 12

  const jsDay = date?.getDay()                 // 0 (Chủ nhật) → 6 (Thứ Bảy)
  const dayOfWeek = jsDay === 0 ? 7 : jsDay    // map 0 → 7

  return `${minutes} ${hours} ${dayOfMonth} ${month} ${dayOfWeek}`
}

//THINGSBOARD
// ** Auth
export async function loginTb(): Promise<LoginResult> {
  try {
    const username = process.env.TB_USERNAME
    const password = process.env.TB_PASSWORD

    if (!username || !password) {
      throw new Error(
        'The environment variables for the username and password are not defined.'
      )
    }

    const responseLogin: AxiosResponse<ResponseData> = await axios.post(
      `/api/auth/login`,
      {
        username,
        password
      }
    )

    const { token, refreshToken } = responseLogin.data
    return { token, refreshToken }
  } catch (error) {
    return { error: error as Error }
  }
}

// ** Devices

export async function getDeviceInfo(deviceId: string, token: string) {
  try {

    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`
      }
    }

    const response = await axios.get(
      `/api/device/${deviceId}`, header
    )

    return response.data
  } catch (error) {
    return { error: error as Error };
  }
}

export async function getDeviceListInfo(pageSize: number, page: number, token: string) {
  try {

    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`
      }
    }

    const params = {
      pageSize: pageSize,
      page: page
    }

    const response = await axios.get(
      `/api/customer/${process.env.TB_CUSTOMER_ID}/devices`,
      {
        params,
        ...header,
      }
    )

    return response.data.data
  } catch (error) {
    return { error: error as Error };
  }
}

export async function addDevice(deviceName: string, token: string, accessToken: string) {
  try {

    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`
      }
    }

    const data = {
      "device": {
        "name": deviceName,
        "label": `Label_${deviceName}`,
        "type": "default",
        "deviceProfileId": {
          "id": `${process.env.TB_DEVICE_PROFILE_ID}`,
          "entityType": "DEVICE_PROFILE"
        },
        "customerId": {
          "id": `${process.env.TB_CUSTOMER_ID}`,
          "entityType": "CUSTOMER"
        }
      },
      "credentials": {
        "credentialsType": "ACCESS_TOKEN",
        "credentialsId": `${accessToken}`
      }
    }

    const response = await axios.post(
      `/api/device-with-credentials`, data, header
    )
    return response.data
  } catch (error) {
    return { error: error as Error };
  }
}

export async function deleteDevice(deviceId: string, token: string) {
  try {

    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`
      }
    }

    const response = await axios.delete(`api/device/${deviceId}`, header)
    return response.data

  } catch (error) {
    return { error: error as Error };
  }
}

export async function sendDataTb(key: string, method: string, token: string, deviceId: string, id: number, date?: Date | undefined, time?: string | undefined, data?: boolean) {
  try {
    if (!token) {
      throw new Error('Token of ThingsBoard is required.')
    }

    let body = {}

    switch (method) {
      case 'delete':
        body = {
          [`schedule/${method}`]: `${key}1`,
          "id": id,
        }
        break;
      case 'ota':
        body = {
          [`${method}/fw_title`]: key,
          'fw_version': id
        }
        break;
      case 'control':
        body = {
          [`control/${key}`]: `${(key == 'measurement_time' ? (id * 1000) : (id == 1 ? 'ON' : 'OFF'))}`
        }
        break;
      default:
        body = {
          [`schedule/${method}`]: `${key}1`,
          "id": id,
          "cron": dateAndTimeToCronWithSeconds(date, time as string),
          "state": data ? true : false
        }
        break;
    }

    let dataSent = {
      "method": "toggleRelay",
      "params": body
    }

    // const dataSent = {
    //   [key]: data
    // }
    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`
      }
    }
    const response = await axios.post(`/api/plugins/rpc/twoway/${deviceId}`, dataSent, header)
    return response.data
  } catch (error) {
    return { error: error as Error }
  }
}

export async function getTelemetryTb(deviceId: string, token: string, keys?: string[]): Promise<any> {
  try {
    if (!token) {
      throw new Error('Token of ThingsBoard is required.');
    }

    if (!deviceId) {
      throw new Error('Device ID is required.');
    }

    const params = {
      keys: keys ? keys.join(',') : undefined,
    };

    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`,
      },
    };

    const response = await axios.get(
      `/api/plugins/telemetry/DEVICE/${deviceId}/values/timeseries`,
      {
        params,
        ...header,
      }
    );

    return response.data;
  } catch (error) {
    console.error('Error fetching telemetry data:', error);
    return { error: error as Error };
  }
}

export async function getHistoricalDataTb(token: string, deviceId: string, keys: string, startTs: string, endTs: string, interval?: string, agg?: string, limit?: string) {
  try {
    const header = {
      headers: {
        "Content-Type": "application/json",
        "Accept": "application/json",
        'X-Authorization': `Bearer ${token}`,
      }
    }

    const params: any = {
      keys,
      startTs,
      endTs
    }

    if (interval) params['interval'] = interval
    if (limit) params['limit'] = limit
    if (agg) params['agg'] = agg

    const response = await axios.get(
      `/api/plugins/telemetry/DEVICE/${deviceId}/values/timeseries`,
      {
        params,
        ...header,
      }
    );

    return response.data;
  } catch (error) {
    console.error('Error fetching telemetry data:', error);
    return { error: error as Error };
  }

}

// * SENSOR RANGE
export async function addRange(task: any) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase.from("sensor_ranges").insert({ ...task }).single()

  return result
}

export async function addDefaultRange(deviceId: string) {
  const supabase = await createSupabaseServerClient()

  const defaults = ['pH', 'ORP', 'TUR', 'EC'].map((sensor) => {
    return {
      deviceId,
      sensor,
      minValue: -999999999,
      maxValue: 999999999,
      isCreatedByUser: false
    }
  })

  const result = await supabase.from("sensor_ranges").insert({ ...defaults }).single()

  return result
}

export async function getRanges(sensor?: string, deviceId?: string) {
  noStore()
  const supabase = await createSupabaseServerClient()

  let q = supabase
    .from('sensor_ranges')
    .select('*').eq('isCreatedByUser', true)
    .order('created_at', { ascending: false })

  if (deviceId) q = q.eq('deviceId', deviceId)

  if (sensor) q = q.eq('sensor', sensor)

  return await q
}

export async function updateRanges(deviceId: string, range: any, sensor?: string, id?: string) {
  const supabase = await createSupabaseServerClient()
  let q = supabase
    .from('sensor_ranges')
    .update(range)
    .eq('deviceId', deviceId)
    .eq('isCreatedByUser', true)

  if (sensor) q = q.eq('sensor', sensor)
  if (id) q = q.eq('id', id)

  return await q
}

export async function deleteRanges(id: string, userId: string) {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('sensor_ranges')
    .delete()
    .eq('id', id)
    .eq('created_by', userId)
}

// * Notifications
export async function addNotification(task: any) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase.from("notifications").insert({ ...task }).single()

  return result
}

export async function getNotifications(deviceId?: string, deviceName?: string, sensor?: string, condition?: string) {
  noStore()
  const supabase = await createSupabaseServerClient()
  let q = supabase
    .from('notifications')
    .select('*')
    .order('created_at', { ascending: false })

  if (deviceId) q = q.eq('deviceId', deviceId)
  if (deviceName) q = q.eq('deviceName', deviceName)
  if (sensor) q = q.eq('sensor', sensor)
  if (condition) q = q.eq('condition', condition)

  return await q
}

// TASK
export async function addTask(task: any) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase.from("tasks").insert({ ...task }).single()

  return result
}

export async function getTasks(deviceId?: string, id?: string) {
  noStore()
  const supabase = await createSupabaseServerClient()

  let q = supabase
    .from('tasks')
    .select('*')
    .order('created_at', { ascending: false })

  if (deviceId) q = q.eq('deviceId', deviceId)
  if (id) q = q.eq('rpcID', id)
  return await q
}

export async function updateTask(id: string, userId: string, task: TaskData) {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('tasks')
    .update(task)
    .eq('id', id)
    .eq('created_by', userId)
}

export async function updateTaskStatus(id: string, userId: string, status: string) {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('tasks')
    .update({ status })
    .eq('rpcID', id)
    .eq('created_by', userId)
}

export async function deleteTask(id: string, userId: string) {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('tasks')
    .delete()
    .eq('id', id)
    .eq('created_by', userId)
}

//SETTING

export async function createSetting(data: {
  entityType: string
  entityId: string
}) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase
    .from('settings')
    .insert({
      entityType: data.entityType,
      entityId: data.entityId
    })
    .single()

  revalidatePath('/')
  return result
}

export async function createSettingv2(data: {
  ownerId: string
  entityType: string
  entityId: string
}) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase
    .from('settings')
    .insert({
      ownerId: data.ownerId,
      entityType: data.entityType,
      entityId: data.entityId
    })
    .single()

  return result
}

export async function readSetting() {
  noStore()
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('settings')
    .select('*')
    .order('created_at', { ascending: false })
    .limit(1)
}

//AUTH
export async function signUpWithEmailAndPassword(data: {
  email: string
  password: string
  confirm: string
}) {
  const supabase = await createSupabaseServerClient()
  const result = await supabase.auth.signUp({
    email: data.email,
    password: data.password
  })
  return JSON.parse(JSON.stringify(result))
}

export async function signInWithEmailAndPassword(data: {
  email: string
  password: string
}) {
  const supabase = await createSupabaseServerClient()
  const result = await supabase.auth.signInWithPassword({
    email: data.email,
    password: data.password
  })
  return JSON.parse(JSON.stringify(result))
}

export async function signOut() {
  const supabase = await createSupabaseServerClient()
  await supabase.auth.signOut()
  redirect('/auth')
}

export default async function readUserSession() {
  const supabase = await createSupabaseServerClient()
  return supabase.auth.getSession()
}
