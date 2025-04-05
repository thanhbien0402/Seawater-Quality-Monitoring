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
  task_date: Date
  task_time: string
  trigger_type: boolean
}

//THINGSBOARD
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

export async function sendDataTb(key: string, data: boolean, token: string) {
  try {
    if (!token) {
      throw new Error('Token of ThingsBoard is required.')
    }

    const dataSent = {
      "method": "toggleRelay",
      "params": {
        [key]: data ? "ON" : "OFF"
      }
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
    await axios.post(`/api/plugins/rpc/oneway/a6e924c0-9f72-11ef-af67-a38a7671daf5`, dataSent, header)
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

// TASK
export async function addTask(task: any) {
  const supabase = await createSupabaseServerClient()

  const result = await supabase.from("tasks").insert({ ...task }).single()

  return result
}

export async function getTasks() {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('tasks')
    .select('*')
    .order('created_at', { ascending: false })
}

export async function updateTask(id: string, userId: string, task: TaskData) {
  const supabase = await createSupabaseServerClient()
  return await supabase
    .from('tasks')
    .update(task)
    .eq('id', id)
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
